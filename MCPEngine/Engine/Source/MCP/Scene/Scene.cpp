// Scene.cpp

#include "Scene.h"

#include <algorithm>

#include "IRenderable.h"
#include "MCP/Objects/Object.h"
#include "MCP/Application/Resources/PackageManager.h"
#include "MCP/Components/ComponentFactory.h"
#include "MCP/Components/TransformComponent.h"
#include "MCP/Scene/IUpdateable.h"
#include "Platform/TinyXML2/tinyxml2.h"

namespace mcp
{
    Scene::Scene()
        : m_updateables(50)  // These are all magic numbers. I need a better solution for the containers.
        , m_fixedUpdateables(50)
        , m_worldRenderables(10)
        , m_objectRenderables(100)
        , m_debugOverlayRenderables(100)
        , m_collisionSystem(QuadtreeBehaviorData{ 4, 4, 1600.f, 900.f })
        , m_accumulatedTime(0.f)
        , m_transitionQueued(false)
    {
        //
    }

    Scene::~Scene()
    {
        ClearScene();
    }

    bool Scene::Init()
    {
        return true;
    }

    bool Scene::Load(const char* pFilePath)
    {
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(pFilePath) != tinyxml2::XML_SUCCESS)
        {
            LogError("Failed to load Scene! XML Error: %", tinyxml2::XMLDocument::ErrorIDToName(doc.ErrorID()));
            return false;
        }

        // Check to see if we need to load any packages for this Scene
        const auto* pPackageElement = doc.FirstChildElement(kPackageElementName);
        if (pPackageElement)
        {
            // Get the Package file path:
            const char* pPackageFilePath = pPackageElement->Attribute("path");
            if (!pPackageFilePath)
            {
                LogError("Failed to load scene! Couldn't find Package path in xml file!");
                return false;
            }

            // Load the package.
            if (!PackageManager::Get()->LoadPackage(pPackageFilePath))
            {
                LogError("Failed to load scene! Couldn't load package defined in xml 'path' attribute");
                return false;
            }
        }

        // Get the Scene Element.
        const auto* pSceneElement = doc.FirstChildElement(kSceneElementName);
        if (!pSceneElement)
        {
            // Oh no the formatting of the xml document isn't standardized!
            LogError("Failed to load scene! Couldn't find <%> Element!", kSceneElementName);
            return false;
        }

        // Get the first Object element.
        auto* pObjectElement = pSceneElement->FirstChildElement(kObjectElementName);

        while (pObjectElement)
        {
            // Create a new object and add it to the scene.
            auto* pObject = CreateObject();

            // Get the first Component of the object.
            auto* pComponentElement = pObjectElement->FirstChildElement();

            while (pComponentElement)
            {
                // Add the Component to the new Object.
                if (!ComponentFactory::AddToObjectFromData(pComponentElement->Value(), pComponentElement, pObject))
                {
                    ClearScene();
                    return false;
                }

                // Go to the next sibling to see if we have another component.
                pComponentElement = pComponentElement->NextSiblingElement();
            }

            // Go to the next sibling to see if we have another gameObject.
            pObjectElement = pObjectElement->NextSiblingElement(kObjectElementName);
        }

        // Everything succeeded!
        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : OnSceneLoad is called once the scene has completed loading. All objects that are in the original scene are
    ///             guaranteed to be completely loaded. The scene will notify all objects so they can perfom
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Scene::OnSceneLoad()
    {
        for (auto& [id, pObject] : m_objects)
        {
            if (!pObject->PostLoadInit())
            {
                // Log an error, but don't 
                LogError("Object failed to successfully initialize!");
                return false;
            }
        }

        // Create our collision grid.
        //m_collisionSystem.CalculateGrid();

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      TODO: We should time slice this!
    //
    ///		@brief : Update every Updateable in the Scene based on deltaTime.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Scene::Update(const float deltaTime)
    {
        m_messageManager.ProcessMessages();

        const auto& updateableArray = m_updateables.GetArray();

        for (auto* pUpdateable : updateableArray)
        {
            pUpdateable->Update(deltaTime);

            if (m_transitionQueued)
                break;
        }

        // If enough time has passed, perform a fixed update.
        m_accumulatedTime += deltaTime;
        if (m_accumulatedTime > kFixedUpdateTimeSeconds)
        {
            m_accumulatedTime -= kFixedUpdateTimeSeconds;

            const auto& fixedUpdateables = m_fixedUpdateables.GetArray();

            for (auto* pUpdateable : fixedUpdateables)
            {
                pUpdateable->Update(kFixedUpdateTimeSeconds);

                // Can this happen for physics objects?
                if (m_transitionQueued)
                    break;
            }
        }

        DeleteQueuedObjects();

        // Run Collisions, after all of the updates have gone through.
        m_collisionSystem.RunCollisions();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Render each Renderable in the Scene.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Scene::Render() const
    {
        RenderLayer(m_worldRenderables);
        RenderLayer(m_objectRenderables);
        RenderLayer(m_debugOverlayRenderables);

#if DEBUG_RENDER_COLLISION_TREE
        m_collisionSystem.Render();
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Create a new Object in the Scene.
    //-----------------------------------------------------------------------------------------------------------------------------
    Object* Scene::CreateObject()
    {
        auto* pNewObject = BLEACH_NEW(Object(this));
        m_objects.emplace(pNewObject->GetId(), pNewObject);

        return pNewObject;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Queues an Object for destruction. The object will be deleted after the update loop has finished.
    ///		@param id : Id of the Object you want to destroy.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Scene::DestroyObject(const ObjectId id)
    {
        const auto result = m_objects.find(id);
        if (result == m_objects.end())
        {
            LogWarning("Failed to Destroy Object! Object Id was invalid.");
            return;
        }

        m_queuedObjectsToDelete.emplace_back(result->first);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Checks to see if the ObjectId points to a valid Object or not.
    ///		@returns : True if there is an Object with that Id in the scene.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Scene::IsValidId(const ObjectId id)
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
    ///		@brief : Add a Renderable to the scene.
    ///		@param pRenderable : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void Scene::AddRenderable(IRenderable* pRenderable)
    {
        switch(pRenderable->GetRenderLayer())
        {
            case RenderLayer::kWorld:
            {
                m_worldRenderables.Add(pRenderable->GetRenderId(), pRenderable);
                break;
            }

            case RenderLayer::kObject:
            {
                m_objectRenderables.Add(pRenderable->GetRenderId(), pRenderable);
                break;
            }

            case RenderLayer::kDebugOverlay:
            {
                m_debugOverlayRenderables.Add(pRenderable->GetRenderId(), pRenderable);
                break;
            }
        }
    }

    void Scene::RemoveRenderable(const IRenderable* pRenderable)
    {
        switch(pRenderable->GetRenderLayer())
        {
            case RenderLayer::kWorld:
            {
                m_worldRenderables.Remove(pRenderable->GetRenderId());
                break;
            }

            case RenderLayer::kObject:
            {
                m_objectRenderables.Remove(pRenderable->GetRenderId());
                break;
            }

            case RenderLayer::kDebugOverlay:
            {
                m_debugOverlayRenderables.Remove(pRenderable->GetRenderId());
                break;
            }
        }
    }

    void Scene::AddUpdateable(const ObjectId id, IUpdateable* pUpdateable)
    {
        m_updateables.Add(id, pUpdateable);
    }

    void Scene::RemoveUpdateable(const ObjectId id)
    {
        m_updateables.Remove(id);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Add an updateable that will only tick on a fixed interval.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Scene::AddPhysicsUpdateable(IUpdateable* pUpdateable)
    {
        m_fixedUpdateables.Add(pUpdateable->GetUpdateId(), pUpdateable);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Remove an updateable that was only ticking on a fixed interval. You would to have needed to call 'AddPhysicsUpdateable'
    ///         for this function to succeed.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Scene::RemovePhysicsUpdateable(const IUpdateable* pUpdateable)
    {
        m_fixedUpdateables.Remove(pUpdateable->GetUpdateId());
    }

    void Scene::RenderLayer(const RenderableContainer& renderables) const
    {
        static std::vector<IRenderable*> renderableArray;

        // Copy of the array :(
        renderableArray = renderables.GetArray();

        // Sort the array based on the zOrder.
        std::sort(renderableArray.begin(), renderableArray.end(), [](const IRenderable* pLeft, const IRenderable* pRight)
        {
            return pLeft->GetZOrder() > pRight->GetZOrder();
        });

        // Render each renderable. Virtual call :(
        for (const auto* pRenderable : renderableArray)
        {
            pRenderable->Render();
        }
    }


    void Scene::DeleteQueuedObjects()
    {
        for (const auto ObjectId : m_queuedObjectsToDelete)
        {
            auto result = m_objects.find(ObjectId);

            BLEACH_DELETE(result->second);
            m_objects.erase(result);
        }

        m_queuedObjectsToDelete.clear();
    }

    void Scene::ClearScene()
    {
        for (const auto&[id, pObject] : m_objects)
        {
            BLEACH_DELETE(pObject);
        }

        m_objects.clear();
    }
}
