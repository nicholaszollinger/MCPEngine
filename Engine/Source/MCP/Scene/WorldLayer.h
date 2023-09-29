#pragma once
// WorldLayer.h

#include "MCP/Collision/CollisionSystem.h"
#include "MCP/Core/Event/MessageManager.h"
#include "MCP/Scene/Object.h"
#include "MCP/Scene/SceneLayer.h"

namespace mcp
{
    class WorldLayer final : public SceneLayer
    {
        std::unordered_map<UpdateableId, Object*> m_objects;             // Container of all of the Object's in the scene.
        std::vector<UpdateableId> m_queuedObjectsToDelete;               // Objects that will be deleted at the end of the update.
        CollisionSystem m_collisionSystem;
        MessageManager m_messageManager;

    public:
        WorldLayer(Scene* pScene);
        virtual ~WorldLayer() override; 

        virtual void Update(const float deltaTimeMs) override;
        virtual void FixedUpdate(const float fixedUpdateTimeS) override;
        virtual void Render() override;
        virtual void OnEvent(const ApplicationEvent& event) override;
        virtual bool OnSceneLoad() override;

        // Object Management
        Object* CreateObject();
        void DestroyObject(const ObjectId id);
        [[nodiscard]] bool IsValidId(const ObjectId id);

        // World Systems
        [[nodiscard]] MessageManager* GetMessageManager() { return &m_messageManager;}

        void SetCollisionSettings(const QuadtreeBehaviorData& data);
        [[nodiscard]] CollisionSystem* GetCollisionSystem() { return &m_collisionSystem;}

    private:
        void DeleteQueuedObjects();
        void DestroyWorld();
    };
}
