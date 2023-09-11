#pragma once
// SceneManager.h
#include "Scene.h"
#include "MCP/Application/Core/GlobalManager.h"

namespace mcp
{
    class SceneManager final : public IProcess
    {
        using SceneIdentifier = const char*;

        DEFINE_GLOBAL_MANAGER(SceneManager)
            
        Scene* m_pActiveScene;
        SceneIdentifier m_sceneToTransitionTo;
        bool m_transitionQueued;

    public:
        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager(SceneManager&&) = delete;
        SceneManager& operator=(SceneManager&&) = delete;
    private:
        SceneManager();
        
    public:
        // This should get the Default scene ready.
        bool LoadSceneData(const char* pSceneFilePath);
        void Update(const float deltaTime);
        void Render() const;
        void QueueTransition(const SceneIdentifier& identifier); // Needs some kind of identifier to designate what scene to go to.

        // TODO: TEMPORARY SOLUTION. WE SHOULD BE SETTING SCENE INFO FROM DATA.
        void SetScene(Scene* pScene) { m_pActiveScene = pScene; }

        [[nodiscard]] Scene* GetActiveScene() const { return m_pActiveScene; }

    private:
        virtual bool Init() override;
        virtual void Close() override;
        bool TransitionToScene();

    };
}