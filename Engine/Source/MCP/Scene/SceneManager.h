#pragma once
// SceneManager.h
#include "Scene.h"
#include "MCP/Core/System.h"

struct lua_State;

namespace mcp
{
    class SceneEntity;
    using SceneIdentifier = StringId;

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

    class SceneManager final : public System
    {
        using SceneList = std::unordered_map<SceneIdentifier, SceneData, StringIdHasher>;

        MCP_DEFINE_SYSTEM(SceneManager)
        SceneManager(SceneList&& sceneList, const SceneIdentifier startScene);

        SceneList m_sceneList;
        Scene* m_pActiveScene = nullptr;
        SceneIdentifier m_startScene;
        SceneIdentifier m_sceneToTransitionTo;

#if MCP_EDITOR
        const SceneIdentifier m_editorScene = "EditorScene";
        XMLParser m_loadedAsset;
#endif

        bool m_transitionQueued = false;
        
    public:
        void QueueTransition(const SceneIdentifier& identifier);
        [[nodiscard]] Scene* GetActiveScene() const { return m_pActiveScene; }

#if MCP_EDITOR
        // TODO:
        //void MarkSceneDirty() {}
#endif

        static SceneManager* Get();
        static SceneManager* AddFromData(const XMLElement element);
        static void RegisterLuaFunctions(lua_State* pState);

    private:
        virtual bool Init() override;
        virtual void Close() override;
        bool EnterStartScene();
        void Update(const float deltaTimeMs);
        void Render() const;
        bool TransitionToScene();

#if MCP_EDITOR
        bool LoadEditorScene();
#endif
    };
}