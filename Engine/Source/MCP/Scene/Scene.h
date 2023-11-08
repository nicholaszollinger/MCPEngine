#pragma once
// Scene.h
#include "IRenderable.h"
#include "IUpdateable.h"
#include "MCP/Collision/CollisionSystem.h"
#include "MCP/Core/Event/MessageManager.h"
#include "MCP/Scene/UILayer.h"
#include "MCP/Scene/WorldLayer.h"

namespace mcp
{
    class SceneEntity;

    class Scene
    {
    private:
        static constexpr const char* kPackageElementName = "Package";
        static constexpr const char* kSceneElementName = "Scene";
        static constexpr float kFixedUpdateTimeSeconds = 1.f / 60.f;

        MessageManager m_messageManager;
        WorldLayer* m_pWorldLayer;
        UILayer* m_pUILayer;
        float m_accumulatedTime;                         // Amount of time before we perform a fixed update.
        bool m_transitionQueued;                         // Whether a scene transition has been queued or not.
        bool m_isLoaded;

    public:
        Scene();
        ~Scene();

        bool Load(const char* pFilePath);
        bool OnSceneLoad();
        void Destroy();
        void Update(const float deltaTime);
        void Render() const;

        [[nodiscard]] MessageManager* GetMessageManager();
        [[nodiscard]] WorldLayer* GetWorldLayer() const { return m_pWorldLayer; }
        [[nodiscard]] UILayer* GetUILayer() const { return m_pUILayer; }
        [[nodiscard]] bool TransitionQueued() const { return m_transitionQueued; }

    private:
        bool Init();
        void OnEvent(ApplicationEvent& event) const;
    };
}