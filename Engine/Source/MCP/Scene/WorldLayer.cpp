// WorldLayer.cpp

#include "WorldLayer.h"

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
        DestroyWorld();
    }

    bool WorldLayer::OnSceneLoad()
    {
        for (auto& [id, pObject] : m_objects)
        {
            if (!pObject->PostLoadInit())
            {
                // Log an error, but don't 
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

        DeleteQueuedObjects();
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

    Object* WorldLayer::CreateObject()
    {
        auto* pNewObject = BLEACH_NEW(Object(m_pScene));
        m_objects.emplace(pNewObject->GetId(), pNewObject);

        return pNewObject;
    }

    void WorldLayer::DestroyObject(const UpdateableId id)
    {
        const auto result = m_objects.find(id);
        if (result == m_objects.end())
        {
            MCP_WARN("World", "Failed to Destroy Object! Object Id was invalid.");
            return;
        }

        m_queuedObjectsToDelete.emplace_back(result->first);
    }

    bool WorldLayer::IsValidId(const UpdateableId id)
    {
        if (const auto result = m_objects.find(id); result == m_objects.end())
        {
            return false;
        }

        return true;
    }

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

    void WorldLayer::DestroyWorld()
    {
        for (const auto&[id, pObject] : m_objects)
        {
            BLEACH_DELETE(pObject);
        }

        m_objects.clear();
    }

    void WorldLayer::OnEvent([[maybe_unused]] const ApplicationEvent& event)
    {
        // TODO: This needs to be called by the Scene, and the Scene needs to register for the application events.

    }

    void WorldLayer::SetCollisionSettings(const QuadtreeBehaviorData& data)
    {
        m_collisionSystem.SetQuadtreeBehaviorData(data);
    }
}