// SceneManager.cpp

#include "SceneManager.h"

#include <BleachNew.h>
#include "LuaSource.h"
#include "SceneAsset.h"
#include "MCP/Core/Application/Application.h"
#include "MCP/Debug/Log.h"
#include "MCP/Graphics/Graphics.h"
#include "Utility/String/Path.h"

namespace mcp
{
    MCP_DEFINE_STATIC_SYSTEM_GETTER(SceneManager)

    SceneManager::SceneManager(SceneList&& sceneList, const SceneIdentifier startScene)
        : m_sceneList(std::move(sceneList))
        , m_startScene(startScene)
    {
        //
    }

    bool SceneManager::Init()
    {
        // Make sure that our start scene is valid.
        const auto result = m_sceneList.find(m_startScene);
        if (result == m_sceneList.end())
        {
            MCP_ERROR("SceneManager", "Failed to initialize SceneManager! Failed to find the start scene in the scene list! StartScene id: ", m_startScene.GetCStr());
            Close();
            return false;
        }

        // Create the Scenes.
        for (auto&[id, sceneData] : m_sceneList)
        {
            sceneData.pScene = BLEACH_NEW(Scene);
        }

#if MCP_EDITOR
        // Check to see if a scene asset path has been passed into the executable:
        auto& args = Application::Get()->GetContext().args;

        // If a path has been passed into the executable, load that scene asset into an empty scene.
        if (args.count > 1)
        {
            const Path path = args.args[1];

            // If this is a valid xml file, load the scene asset into an empty scene..
            if (path.ExtensionMatches(".xml"))
            {
                // Create an empty scene.
                const auto val = m_sceneList.emplace(m_editorScene, SceneData("InvalidPath", BLEACH_NEW(Scene)));
                m_startScene = val.first->first;
            }

            else
            {
                MCP_ERROR("SceneManager", "CommandLineArgument[1] is not a path to an xml file. Opening StartScene...");
                //m_sceneToTransitionTo = m_startScene;
                return false;
            }
        }

        else
        {
            MCP_ERROR("SceneManager", "No Command Line argument was passed into the Editor! Closing the Application...");
            return false;
        }
#endif

        return true;
    }

    void SceneManager::Close()
    {
        if (m_pActiveScene)
            m_pActiveScene->Destroy();

        // Delete all of our scenes.
        for (auto&[id, data] : m_sceneList)
        {
            BLEACH_DELETE(data.pScene);
            data.pScene = nullptr;
        }

        m_pActiveScene = nullptr;
    }

    bool SceneManager::EnterStartScene()
    {
#if MCP_EDITOR
        if (m_startScene == m_editorScene)
        {
            LoadEditorScene();
        }

        else
        {
            m_sceneToTransitionTo = m_startScene;

            if (!TransitionToScene())
            {
                MCP_ERROR("SceneManager", "Failed to transition to the start scene!");
                return false;
            }
        }

#else
        m_sceneToTransitionTo = m_startScene;

        if (!TransitionToScene())
        {
            MCP_ERROR("SceneManager", "Failed to transition to the start scene!");
            return false;
        }

#endif
        return true;
    }

    void SceneManager::Update(const float deltaTimeMs)
    {
        if (m_pActiveScene)
        {
            m_pActiveScene->Update(deltaTimeMs);

            // Only transition to the next scene once the update in the Active is completed.
            if (m_transitionQueued)
            {
                TransitionToScene();
            }
        }
    }

    void SceneManager::Render() const
    {
        if (m_pActiveScene)
        {
            FillScreen(Color{0,0,0});
            m_pActiveScene->Render();
        }
    }

    void SceneManager::QueueTransition(const SceneIdentifier& identifier)
    {
        if (const auto result = m_sceneList.find(identifier); result == m_sceneList.end())
        {
            MCP_WARN("SceneManager", "Attempted to load Scene with no valid identifier! Identifier: ", identifier.GetCStr());
            return;
        }

        m_transitionQueued = true;
        m_sceneToTransitionTo = identifier;
    }

    bool SceneManager::TransitionToScene()
    {
        if (m_pActiveScene)
        {
            //m_pActiveScene->PreDestroy();
            m_pActiveScene->Destroy();
        }

        // This is guaranteed to be valid. Invalid Ids cannot be queued.
        const SceneData& sceneData = m_sceneList[m_sceneToTransitionTo];
        m_pActiveScene = sceneData.pScene;
        MCP_CHECK(m_pActiveScene);

        // Load the Scene
        if (!m_pActiveScene->Load(sceneData.dataPath.c_str()))
        {
            MCP_ERROR("SceneManager", "Failed to Load Scene: ", m_sceneToTransitionTo.GetCStr());
            return false;
        }

        if (!m_pActiveScene->OnSceneLoad())
        {
            MCP_ERROR("SceneManager", "Failed to Load Scene: ", m_sceneToTransitionTo.GetCStr());
            return false;
        }

        m_transitionQueued = false;

        m_pActiveScene->Begin();

        return true;
    }

#if MCP_EDITOR
    bool SceneManager::LoadEditorScene()
    {
        m_pActiveScene = m_sceneList[m_editorScene].pScene;

        const auto& context = Application::Get()->GetContext();
        std::string absolutePath = context.args[1];
        const auto requestPath = absolutePath.erase(0, context.workingDirectory.size());
        //XMLParser m_loadedAsset;
        if (!m_pActiveScene->m_sceneFile.LoadFile(requestPath.c_str()))
        {
            MCP_ERROR("SceneManager", "Failed to load editor scene!");
            return false;
        }

        // HACK:
        // Right now, I am fulfilling the requirements of the assignment. In editor mode,
        // I would want to be loading a scene, but the assignment wants to load a specific UI menu
        // for editing. So I am just doing that.
        const auto root = m_pActiveScene->m_sceneFile.GetElement();
        m_pActiveScene->InitEmpty();
        m_pActiveScene->GetUILayer()->LoadLayerAssetsAndEntities(root);
        //m_pActiveScene->m_sceneFile = m_loadedAsset;
        if (!m_pActiveScene->OnSceneLoad())
        {
            MCP_ERROR("SceneManager", "Failed to load EditorScene! Failed OnSceneLoad!");
            return false;
        }

        m_transitionQueued = false;

        m_pActiveScene->Begin();

        return true;
    }
#endif

    SceneManager* SceneManager::AddFromData(const XMLElement element)
    {
        SceneList sceneList;

        const XMLElement sceneData = element.GetChildElement("SceneData");
        if (!sceneData.IsValid())
        {
            MCP_ERROR("SceneManager", "Failed to load scene data! Failed to find Scene Data element!");
            return nullptr;
        }

        // Get the Scene List.
        const XMLElement sceneListElement = sceneData.GetChildElement("SceneList");
        if (!sceneListElement.IsValid())
        {
            MCP_ERROR("SceneManager", "Failed to load scene data! Couldn't find SceneList element!");
            return nullptr;
        }

        // Create the SceneData for each scene.
        XMLElement scene = sceneListElement.GetChildElement("Scene");
        while (scene.IsValid())
        {
#ifndef _DEBUG
            if (AssetIsDebugOnly(scene))
            {
                scene = scene.GetSiblingElement("Scene");
                continue;
            }
#endif

            const SceneIdentifier id = scene.GetAttributeValue<const char*>("sceneId");
            if (!id.IsValid())
            {
                MCP_ERROR("SceneManager", "Failed to create Scene! No SceneId found!");
                return nullptr;
            }

            auto* path = scene.GetAttributeValue<const char*>("sceneDataPath");
            if (!path)
            {
                MCP_ERROR("SceneManager", "Failed to create Scene! No SceneId found!");
                return nullptr;
            }

            //auto* pScene = BLEACH_NEW(Scene); // Create a new scene.
            sceneList.emplace(id, SceneData(path, nullptr));

            scene = scene.GetSiblingElement("Scene");
        }

        // Get the Start Scene
        const XMLElement startSceneElement = sceneData.GetChildElement("StartScene");
        if (!startSceneElement.IsValid())
        {
            MCP_ERROR("SceneManager", "Failed to find Start Scene!");
            return nullptr;
        }

#if MCP_EDITOR
        auto* startScene = startSceneElement.GetAttributeValue<const char*>("editor");
#else
        auto* startScene = startSceneElement.GetAttributeValue<const char*>("default");
#endif
        if (!startScene)
        {
            MCP_ERROR("SceneManager", "Failed to find Start!");
            return nullptr;
        }

        return BLEACH_NEW(SceneManager(std::move(sceneList), startScene));
    }

    static int ScriptTransitionToScene(lua_State* pState)
    {
        auto* pSceneId = lua_tostring(pState, -1);
        lua_pop(pState, 1);

        SceneManager::Get()->QueueTransition(pSceneId);

        return 0;
    }

    static int PauseWorld([[maybe_unused]] lua_State* pState)
    {
        auto* pScene = SceneManager::Get()->GetActiveScene();
        MCP_CHECK(pScene);

        pScene->GetWorldLayer()->Pause();
        return 0;
    }

    static int ResumeWorld([[maybe_unused]] lua_State* pState)
    {
        auto* pScene = SceneManager::Get()->GetActiveScene();
        MCP_CHECK(pScene);

        pScene->GetWorldLayer()->Resume();
        return 0;
    }

    void SceneManager::RegisterLuaFunctions(lua_State* pState)
    {
        static constexpr luaL_Reg kFuncs[]
        {
             {"TransitionToScene", &ScriptTransitionToScene}
             ,{"PauseWorld", &PauseWorld}
             ,{"ResumeWorld", &ResumeWorld}
            ,{nullptr, nullptr}
        };

        // Set the SceneManager Functions
        lua_getglobal(pState, "SceneManager");
        MCP_CHECK(lua_type(pState, -1) == LUA_TTABLE);
        luaL_setfuncs(pState, kFuncs, 0);
        // Pop the table off the stack.
        lua_pop(pState, 1);
    }
}

