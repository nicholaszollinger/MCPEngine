// WorldLayer.cpp

#include "WorldLayer.h"

#include "MCP/Graphics/Graphics.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    WorldLayer::WorldLayer(Scene* pScene)
        : SceneLayer(pScene)
        , m_collisionSystem(QuadtreeBehaviorData{ 4, 4, 1600.f, 900.f }) // Load this from data
    {
        //
    }

    WorldLayer::~WorldLayer()
    {
        DestroyLayer();
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

        // Elements:
        XMLElement element = layer.GetChildElement(kObjectElementName);
        while(element.IsValid())
        {
            if (HashString32(element.GetName()) == kSceneLayerAssetId)
            {
                LoadSceneDataAsset(element);
            }

            else
            {
                LoadObject(element);   
            }

            element = element.GetSiblingElement();
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Loads an object and all of its components. If loading a component fails, this will queue the object for destruction
    ///         and return.
    //-----------------------------------------------------------------------------------------------------------------------------
    void WorldLayer::LoadObject(const XMLElement element)
    {
        auto* pObject = CreateObject();

        // Get the first Component of the object.
        XMLElement componentElement = element.GetChildElement();

        while (componentElement.IsValid())
        {
            // Create the Component
            auto* pComponent = ComponentFactory::CreateFromData(componentElement.GetName(), componentElement);

            if (!pComponent)
            {
                MCP_ERROR("WorldLayer", "Failed to load object!");
                // Delete the created object
                pObject->Destroy();
                return;
            }

            // Add the Component to the new Object.
            pObject->AddComponent(pComponent);

            // Go to the next sibling to see if we have another component.
            componentElement = componentElement.GetSiblingElement();
        }
    }

    void WorldLayer::LoadSceneDataAsset(const XMLElement sceneDataAsset)
    {
        const char* pPath = sceneDataAsset.GetAttributeValue<const char*>("path");
        MCP_CHECK_MSG(pPath, "Failed to load SceneDataAsset on the UI Layer! No path was found!");

        XMLParser parser;
        if (!parser.LoadFile(pPath))
        {
            MCP_ERROR("UILayer", "Failed to load SceneDataAsset at path: ", pPath);
            return;
        }

        XMLElement assetElement = parser.GetElement();
        while(assetElement.IsValid())
        {
            if (HashString32(assetElement.GetName()) == kSceneLayerAssetId)
            {
                LoadSceneDataAsset(assetElement);
            }

            else
            {
                LoadObject(assetElement);   
            }

            assetElement = assetElement.GetSiblingElement();
        }
    }

    bool WorldLayer::OnSceneLoad()
    {
        for (auto& [id, pObject] : m_objects)
        {
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
        m_messageManager.ProcessMessages();

        const auto& updateableArray = m_updateables.GetArray();

        for (auto* pUpdateable : updateableArray)
        {
            pUpdateable->Update(deltaTimeMs);

            if (m_pScene->TransitionQueued()) // Get this from the Scene.
                break;
        }

        DeleteQueuedObjects();

        // Run Collisions, after all of the updates have gone through.
        m_collisionSystem.RunCollisions();
    }

    void WorldLayer::FixedUpdate(const float fixedUpdateTimeS)
    {
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
    ///		@brief : Create a new, empty object in the World.
    ///		@returns : Pointer to the newly created object.
    //-----------------------------------------------------------------------------------------------------------------------------
    Object* WorldLayer::CreateObject()
    {
        auto* pNewObject = BLEACH_NEW(Object(m_pScene));
        m_objects.emplace(pNewObject->GetId(), pNewObject);

        return pNewObject;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Create a new object in the World from a Prefab (XML file).
    ///		@param pPrefabPath : Path to the prefab. 
    ///		@returns : Pointer to the new object, or nullptr if it failed. (Like the path is wrong or something.
    //-----------------------------------------------------------------------------------------------------------------------------
    Object* WorldLayer::CreateObjectFromPrefab(const char* pPrefabPath)
    {
        XMLParser parser;
        if (!parser.LoadFile(pPrefabPath))
        {
            MCP_ERROR("World", "Failed to create Object from Data! Failed to find prefab file at path: ", pPrefabPath);
            return nullptr;
        }

        const XMLElement element = parser.GetElement("Object");
        if (!element.IsValid())
        {
            MCP_ERROR("World", "Failed to create Object from Data! Failed 'Object' element in file at path: ", pPrefabPath);
            return nullptr;
        }

        // Create the new Object.
        auto* pObject = CreateObject();

        // Load the Components for the object:
        XMLElement componentElement = element.GetChildElement();

        while (componentElement.IsValid())
        {
            auto* pComponent = ComponentFactory::CreateFromData(componentElement.GetName(), componentElement);

            if (!pComponent)
            {
                MCP_ERROR("WorldLayer", "Failed to load object! Failed to add Component!");
                // Delete the created object
                pObject->Destroy();
                return nullptr;
            }

            // Add the Component to the new Object.
            pObject->AddComponent(pComponent);

            // Go to the next sibling to see if we have another component.
            componentElement = componentElement.GetSiblingElement();
        }

        return pObject;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief :  Queues an Object for destruction. The object will be deleted after the update loop has finished.
    ///		@param id : Id of the Object you want to destroy.
    //-----------------------------------------------------------------------------------------------------------------------------
    void WorldLayer::DestroyObject(const ObjectId id)
    {
        const auto result = m_objects.find(id);
        if (result == m_objects.end())
        {
            MCP_WARN("World", "Failed to Destroy Object! Object Id was invalid.");
            return;
        }

        m_queuedObjectsToDelete.emplace_back(result->first);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Checks to see if the ObjectId points to a valid Object or not.
    ///		@param id : Id of the Object you want to destroy.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool WorldLayer::IsValidId(const ObjectId id)
    {
        if (const auto result = m_objects.find(id); result == m_objects.end())
        {
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Deletes every object that has been queued for destruction.
    //-----------------------------------------------------------------------------------------------------------------------------
    void WorldLayer::DeleteQueuedObjects()
    {
        for (const auto objectId : m_queuedObjectsToDelete)
        {
            auto result = m_objects.find(objectId);

            BLEACH_DELETE(result->second);
            m_objects.erase(result);
        }

        m_queuedObjectsToDelete.clear();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Safely destroys the layer.
    //-----------------------------------------------------------------------------------------------------------------------------
    void WorldLayer::DestroyLayer()
    {
        for (const auto&[id, pObject] : m_objects)
        {
            BLEACH_DELETE(pObject);
        }

        m_objects.clear();
    }

    void WorldLayer::OnEvent([[maybe_unused]] ApplicationEvent& event)
    {
        // TODO: This needs to be called by the Scene, and the Scene needs to register for the application events.

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
}