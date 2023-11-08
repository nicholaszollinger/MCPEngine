#pragma once
// SceneManager.h
#include "Scene.h"
#include "MCP/Core/GlobalManager.h"

struct lua_State;

namespace mcp
{
    class SceneEntity;

    class SceneManager final : public IProcess
    {
    public:
        using SceneIdentifier = StringId;

    private:
        struct SceneData
        {
            SceneData() = default;

            SceneData(const char* path, Scene* pScene)
                : dataPath(path)
                , pScene(pScene)
            {
                //
            }

            std::string dataPath;           // Path to the scene data on disk. To be used to load the scene.
            Scene* pScene = nullptr;        // The Scene resource.
        };

        DEFINE_GLOBAL_MANAGER(SceneManager)

        std::unordered_map<StringId, SceneData, StringIdHasher> m_sceneList;
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
        void QueueTransition(const SceneIdentifier& identifier);
        [[nodiscard]] Scene* GetActiveScene() const { return m_pActiveScene; }

        static void RegisterLuaFunctions(lua_State* pState);
    private:
        virtual bool Init() override;
        virtual void Close() override;
        bool LoadSceneData(const char* pSceneDataFilepath);
        void Update(const float deltaTimeMs);
        void Render() const;
        bool TransitionToScene();
    };
}