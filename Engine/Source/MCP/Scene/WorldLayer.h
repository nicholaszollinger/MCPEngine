#pragma once
// WorldLayer.h

#include "MCP/Collision/CollisionSystem.h"
#include "MCP/Core/Event/MessageManager.h"
#include "MCP/Scene/Object.h"
#include "MCP/Scene/SceneLayer.h"

namespace mcp
{
    class InputComponent;

    class WorldLayer final : public SceneLayer
    {
        static constexpr const char* kObjectElementName = "Object";

        std::unordered_map<ObjectId, Object*> m_objects;             // Container of all of the Object's in the scene.
        std::vector<ObjectId> m_queuedObjectsToDelete;               // Objects that will be deleted at the end of the update.
        CollisionSystem m_collisionSystem;
        MessageManager m_messageManager;
        InputComponent* m_activeInput; // Only 1 active input receiver is active at a time.
        bool m_isPaused;

    public:
        WorldLayer(Scene* pScene);
        virtual ~WorldLayer() override; 

        virtual bool LoadLayer(const XMLElement layer) override;
        virtual void Update(const float deltaTimeMs) override;
        virtual void FixedUpdate(const float fixedUpdateTimeS) override;
        virtual void Render() override;
        virtual void OnEvent(ApplicationEvent& event) override;
        virtual bool OnSceneLoad() override;

        // Time
        void Pause();
        void Resume();
        [[nodiscard]] bool IsPaused() const { return m_isPaused; }

        // Input
        void AddInputListener(InputComponent* pInputComponent);
        void RemoveInputListener(InputComponent* pInputComponent);

        // Object Interface
        Object* CreateObject();
        Object* CreateObjectFromPrefab(const char* pPrefabPath);
        void DestroyObject(const ObjectId id);
        bool IsValidId(const ObjectId id);

        // World Systems
        [[nodiscard]] MessageManager* GetMessageManager() { return &m_messageManager;}
        [[nodiscard]] CollisionSystem* GetCollisionSystem() { return &m_collisionSystem;}

    private:
        virtual void LoadSceneDataAsset(const XMLElement sceneDataAsset) override;
        virtual void DestroyLayer() override;
        void LoadObject(const XMLElement element);
        void SetCollisionSettings(const QuadtreeBehaviorData& data);
        void DeleteQueuedObjects();

        // Input
        void TickInput(const float deltaTimeMs);
    };
}
