// WorldLayer.cpp

#include "WorldLayer.h"

#include "SceneAsset.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Scene/Scene.h"
#include "MCP/Components/InputComponent.h"
#include "MCP/Core/Event/ApplicationEvent.h"

namespace mcp
{
    WorldLayer::WorldLayer(Scene* pScene)
        : SceneLayer(pScene)
        , m_collisionSystem(QuadtreeBehaviorData{ 4, 4, 1600.f, 900.f }) // Load this from data
        , m_activeInput(nullptr)
        , m_isPaused(false)
    {
        //
    }

    bool WorldLayer::LoadLayer(const XMLElement layer)
    {
        // Get global settings for the Scene.
        const XMLElement worldSettings = layer.GetChildElement("Settings");
        MCP_CHECK(worldSettings.IsValid()); // This should be an assert.

        // Collision Settings
        XMLElement setting = worldSettings.GetChildElement("Collision");
        MCP_CHECK(setting.IsValid());
        QuadtreeBehaviorData data;
        const auto windowDimensions = GraphicsManager::Get()->GetWindow()->GetDimensions();
        data.worldWidth = setting.GetAttributeValue<float>("worldWidth", static_cast<float>(windowDimensions.width));
        data.worldHeight = setting.GetAttributeValue<float>("worldHeight", static_cast<float>(windowDimensions.height));
        data.worldXPos = setting.GetAttributeValue<float>("worldXPos", 0.f);
        data.worldYPos = setting.GetAttributeValue<float>("worldYPos", 0.f);
        data.maxDepth = setting.GetAttributeValue<unsigned>("maxDepth", 4);
        data.maxObjectsInCell = setting.GetAttributeValue<unsigned>("maxObjectsInCell", 4);
        SetCollisionSettings(data);

        // Entities:
        const XMLElement element = layer.GetChildElement(kEntityElementTag);
        LoadLayerAssetsAndEntities(element);

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Loads an object and all of its components. If loading a component fails, this will queue the object for destruction
    ///         and return.
    //-----------------------------------------------------------------------------------------------------------------------------
    void WorldLayer::LoadEntity(const XMLElement element)
    {
#ifndef _DEBUG
        if (AssetIsDebugOnly(element))
        {
            return;
        }
#endif

        // Get the SceneEntity construction data from the root element.
        const auto cData = SceneEntity::GetEntityConstructionData(element);

        // Create the Object and add it to the layer.
        auto* pObject = BLEACH_NEW(Object(cData));
        AddEntity(pObject);

        // Load each of the components:
        if (!LoadObjectComponents(pObject, element))
        {
            MCP_ERROR("WorldLayer", "Failed to load object from data! Failed to Load component(s)!");
            pObject->Destroy();
            return;
        }

        // Load any Child Objects:
        if (!LoadChildObject(pObject, element))
        {
            MCP_ERROR("WorldLayer", "Failed to load object from data! Failed to load child object(s)!");
            pObject->Destroy();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      This function has to hash each name to check to see if it is a Object or not. If Components were set up like Widgets
    //      in that there was <Component type="TransformComponent">, this could just search for "Component" children. Right now
    //      each element's name is the ComponentType.
    //		
    ///		@brief : Load all components for this Object.
    ///		@param pObject : Object we are loading the Components for.
    ///		@param rootElement : The Object's element in data.
    ///		@returns : False if this failed to load a component.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool WorldLayer::LoadObjectComponents(Object* pObject, const XMLElement rootElement) const
    {
        MCP_CHECK(pObject);
        MCP_CHECK(rootElement.IsValid());

        auto childElement = rootElement.GetChildElement();
        while (childElement.IsValid())
        {
#ifndef _DEBUG
            // Skip any debug components:
            if (AssetIsDebugOnly(childElement))
            {
                childElement = childElement.GetSiblingElement();
                continue;
            }
#endif

            // If this child element is an object, skip
            if (HashString32(childElement.GetName()) == kEntityElementHash)
            {
                childElement = childElement.GetSiblingElement();
                continue;
            }

            // Load the Component
            // Create the Component
            auto* pComponent = ComponentFactory::CreateFromData(childElement.GetName(), childElement);

            if (!pComponent)
            {
                MCP_ERROR("WorldLayer", "Failed to load component!");
                return false;
            }

            // Add the Component to the new Object.
            pObject->AddComponent(pComponent);

            childElement = childElement.GetSiblingElement();
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Recursively loads all child Objects and their components from a parent.
    ///		@returns : False if this or any child object failed to load.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool WorldLayer::LoadChildObject(Object* pParent, const XMLElement parentElement)
    {
        XMLElement childObjectElement = parentElement.GetChildElement(kEntityElementTag);

        // While we have a child:
        while (childObjectElement.IsValid())
        {
#ifndef _DEBUG
            // Skip any debug objects:
            if (AssetIsDebugOnly(childObjectElement))
            {
                childObjectElement = childObjectElement.GetSiblingElement(kEntityElementTag);
                continue;
            }
#endif

            // Get the SceneEntity construction data from the root element.
            const auto cData = SceneEntity::GetEntityConstructionData(childObjectElement);

            // Create the ChildObject and add it to the layer.
            auto* pChildObject = BLEACH_NEW(Object(cData));
            AddEntity(pChildObject);

            // Set the parent
            pChildObject->SetParent(pParent);

            // Load the Components for the object:
            if (!LoadObjectComponents(pChildObject, childObjectElement))
            {
                MCP_ERROR("WorldLayer", "Failed to load Child Object!");
                return false;
            }

            // Load any children that they have:
            if (!LoadChildObject(pChildObject, childObjectElement))
            {
                return false;
            }

            // Get the next child object:
            childObjectElement = childObjectElement.GetSiblingElement(kEntityElementTag);
        }

        return true;
    }

    bool WorldLayer::OnSceneLoad()
    {
        for (auto& [id, pEntity] : m_entities)
        {
            auto* pObject = SceneEntity::SafeCastEntity<Object>(pEntity);
            if (!pObject->PostLoadInit())
            {
                // Should I return false here and stop execution entirely based on an Object failing to initialize???
                MCP_ERROR("WorldLayer", "Object failed to successfully initialize!");
                return false;
            }
        }

        return true;
    }

    void WorldLayer::Update(const float deltaTimeMs)
    {
        if (m_isPaused)
            return;

        // Process Input
        TickInput(deltaTimeMs);

        // Process Messages 
        m_messageManager.ProcessMessages();

        const auto& updateableArray = m_updateables.GetArray();

        for (auto* pUpdateable : updateableArray)
        {
            pUpdateable->Update(deltaTimeMs);

            if (m_pScene->TransitionQueued()) // Get this from the Scene.
                break;
        }

        DeleteQueuedEntities();

        // Run Collisions, after all of the updates have gone through.
        m_collisionSystem.RunCollisions();
    }

    void WorldLayer::FixedUpdate(const float fixedUpdateTimeS)
    {
        if (m_isPaused)
            return;

        const auto& updateableArray = m_fixedUpdateables.GetArray();

        for (auto* pUpdateable : updateableArray)
        {
            pUpdateable->Update(fixedUpdateTimeS);

            if (m_pScene->TransitionQueued()) // Get this from the Scene.
                break;
        }
    }

    void WorldLayer::Render()
    {
        static std::vector<IRenderable*> renderableArray;
        // This is a copy.....
        renderableArray = m_renderables.GetArray();

        // Sort the array based on the zOrder.
        std::sort(renderableArray.begin(), renderableArray.end(), [](const IRenderable* pLeft, const IRenderable* pRight)
        {
            return pLeft->GetZOrder() < pRight->GetZOrder();
        });

        // Render each renderable.
        for (const auto* pRenderable : renderableArray)
        {
            pRenderable->Render();
        }

#if DEBUG_RENDER_COLLISION_TREE
        m_collisionSystem.Render();
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Pause the entire World Layer, stopping the Update and FixedUpdate from occurring.
    //-----------------------------------------------------------------------------------------------------------------------------
    void WorldLayer::Pause()
    {
        m_isPaused = true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Resume the World Layer, allowing the Update and FixedUpdate to resume.
    //-----------------------------------------------------------------------------------------------------------------------------
    void WorldLayer::Resume()
    {
        m_isPaused = false;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Right now, I have a single listener. In the future I would like to have multiple.
    //		
    ///		@brief : Add an Input listener to receive input events.
    //-----------------------------------------------------------------------------------------------------------------------------
    void WorldLayer::AddInputListener(InputComponent* pInputComponent)
    {
        m_activeInput = pInputComponent;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      In the future, if and when I have multiple listeners, I would have this find the specific component.
    //		
    ///		@brief : Remove an Input listener.
    //-----------------------------------------------------------------------------------------------------------------------------
    void WorldLayer::RemoveInputListener([[maybe_unused]] InputComponent* pInputComponent)
    {
        m_activeInput = nullptr;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Creates a new, empty object in the World and adds it to this layer.
    //-----------------------------------------------------------------------------------------------------------------------------
    Object* WorldLayer::CreateEntity()
    {
        auto* pNewObject = BLEACH_NEW(Object(SceneEntityConstructionData()));
        AddEntity(pNewObject);

        return pNewObject;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Create a new object in the World from a Prefab (XML file).
    ///		@param root : "Object" element in the XML File.
    ///		@returns : Pointer to the new object, or nullptr if it failed.
    //-----------------------------------------------------------------------------------------------------------------------------
    Object* WorldLayer::CreateEntityFromPrefab(const XMLElement root)
    {
        MCP_CHECK(root.IsValid());

        // Get the SceneEntity construction data from the root element.
        const auto cData = SceneEntity::GetEntityConstructionData(root);

        // Create the Object and add it to the layer.
        auto* pObject = BLEACH_NEW(Object(cData));
        AddEntity(pObject);

        // Load each of the components:
        if (!LoadObjectComponents(pObject, root))
        {
            MCP_ERROR("WorldLayer", "Failed to load object prefab! Failed to Load component(s)!");
            pObject->Destroy();
            return nullptr;
        }

        // Load any Child Objects:
        if (!LoadChildObject(pObject, root))
        {
            MCP_ERROR("WorldLayer", "Failed to load object prefab! Failed to load child object(s)!");
            pObject->Destroy();
            return nullptr;
        }

        return pObject;
    }

    void WorldLayer::OnEvent([[maybe_unused]] ApplicationEvent& event)
    {
        if (!m_activeInput)
            return;
        
        // If our input component is active, then we need to process the event
        if (m_activeInput->IsActive())
        {
            if (event.GetEventId() == KeyEvent::GetStaticId())
            {
                auto& keyEvent = static_cast<KeyEvent&>(event);

                // If we don't care about that key, return.
                if (!m_activeInput->HasActionForControl(keyEvent.Key()))
                    return;

                //m_activeInput->UpdateKeyState(keyEvent);
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set tuning data for the Collision System.
    //-----------------------------------------------------------------------------------------------------------------------------
    void WorldLayer::SetCollisionSettings(const QuadtreeBehaviorData& data)
    {
        m_collisionSystem.SetQuadtreeBehaviorData(data);
    }

    void WorldLayer::TickInput([[maybe_unused]] const float deltaTimeMs)
    {
        if (m_activeInput)
            m_activeInput->TickInput(deltaTimeMs);
    }
}
