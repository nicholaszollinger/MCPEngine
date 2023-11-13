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
        MCP_DECLARE_LAYER_ENTITY_TYPE(Object)

        CollisionSystem m_collisionSystem;
        MessageManager m_messageManager;
        InputComponent* m_activeInput; // Only 1 active input receiver is active at a time.
        bool m_isPaused;

    public:
        // Time
        void Pause();
        void Resume();
        [[nodiscard]] bool IsPaused() const { return m_isPaused; }

        // Input
        void AddInputListener(InputComponent* pInputComponent);
        void RemoveInputListener(InputComponent* pInputComponent);

        virtual Object* CreateEntity() override;
        virtual Object* CreateEntityFromPrefab(const XMLElement root) override;

        // World Systems
        [[nodiscard]] MessageManager* GetMessageManager() { return &m_messageManager;}
        [[nodiscard]] CollisionSystem* GetCollisionSystem() { return &m_collisionSystem;}

    private:
        // Private Constructor and Destructor
        WorldLayer(Scene* pScene);

        // Layer Loading
        virtual bool LoadLayer(const XMLElement layer) override;
        virtual void LoadEntity(XMLElement element) override;
        bool LoadChildObject(Object* pParent, const XMLElement parentElement);
        bool LoadObjectComponents(Object* pObject, const XMLElement rootElement) const;

        // Scene Layer Loop
        virtual void Update(const float deltaTimeMs) override;
        virtual void FixedUpdate(const float fixedUpdateTimeS) override;
        virtual void Render() override;
        virtual void OnEvent(ApplicationEvent& event) override;
        virtual bool OnSceneLoad() override;

        void SetCollisionSettings(const QuadtreeBehaviorData& data);

        // Input
        void TickInput(const float deltaTimeMs);
    };
}
