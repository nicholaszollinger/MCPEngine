// SceneManager.cpp

#include "SceneManager.h"

#include <BleachNew.h>
#include "LuaSource.h"
#include "MCP/Debug/Log.h"
#include "MCP/Graphics/Graphics.h"

namespace mcp
{
    SceneManager::SceneManager()
        : m_pActiveScene(nullptr)
        , m_sceneToTransitionTo{}
        , m_transitionQueued(false)
    {
        //
    }

    bool SceneManager::Init()
    {
        return true;
    }

    void SceneManager::Close()
    {
        if (m_pActiveScene)
            m_pActiveScene->Unload();

        // Delete all of our scenes.
        for (auto&[id, data] : m_sceneList)
        {
            BLEACH_DELETE(data.pScene);
            data.pScene = nullptr;
        }

        m_pActiveScene = nullptr;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Load the scene data, 
    ///		@param pSceneDataFilepath : 
    ///		@returns : 
    //-----------------------------------------------------------------------------------------------------------------------------
    bool SceneManager::LoadSceneData(const char* pSceneDataFilepath)
    {
        XMLParser parser;
        if (!parser.LoadFile(pSceneDataFilepath))
        {
            MCP_ERROR("SceneManager", "Failed to load scene data! Failed to load SceneData at filepath: ", pSceneDataFilepath);
            return false;
        }

        const XMLElement sceneData = parser.GetElement("SceneData");
        if (!sceneData.IsValid())
        {
            MCP_ERROR("SceneManager", "Failed to load scene data! Failed to find Scene Data element. Filepath: ", pSceneDataFilepath);
            return false;
        }

        // Get the Scene List.
        const XMLElement sceneList = sceneData.GetChildElement("SceneList");
        if (!sceneList.IsValid())
        {
            MCP_ERROR("SceneManager", "Failed to load scene data! Couldn't find SceneList element!");
            return false;
        }

        // Create the SceneData for each scene.
        XMLElement scene = sceneList.GetChildElement("Scene");
        while (scene.IsValid())
        {
            const SceneIdentifier id = scene.GetAttributeValue<const char*>("sceneId");
            if (!id.IsValid())
            {
                MCP_ERROR("SceneManager", "Failed to create Scene! No SceneId found!");
                Close();
                return false;
            }

            auto* path = scene.GetAttributeValue<const char*>("sceneDataPath");
            if (!path)
            {
                MCP_ERROR("SceneManager", "Failed to create Scene! No SceneId found!");
                Close();
                return false;
            }

            auto* pScene = BLEACH_NEW(Scene); // Create a new scene.
            m_sceneList.emplace(id, SceneData(path, pScene));

            scene = scene.GetSiblingElement("Scene");
        }

        // Load the default Scene
        const XMLElement defaultScene = sceneData.GetChildElement("StartScene");
        if (!defaultScene.IsValid())
        {
            MCP_ERROR("SceneManager", "Failed to find Start Scene!");
            // Should I close here? Or just use the start scene as the default?
            Close();
            return false;
        }

        const SceneIdentifier sceneId = defaultScene.GetAttributeValue<const char*>("sceneId");

        const auto result = m_sceneList.find(sceneId);
        if (result == m_sceneList.end())
        {
            MCP_ERROR("SceneManager", "Failed to find Start Scene in SceneList! StartScene id: ", sceneId.GetCStr());
            Close();
            return false;
        }

        m_sceneToTransitionTo = sceneId;
        if (!TransitionToScene())
        {
            MCP_ERROR("SceneManager", "Failed to load the Start Scene!");
            return false;
        }

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
            m_pActiveScene->Unload(); // This isn't great, because I can't asynchronously transition between scenes.
        }

        // This is guaranteed to be valid. Invalid Ids cannot be queued.
        const SceneData& sceneData = m_sceneList[m_sceneToTransitionTo];
        m_pActiveScene = sceneData.pScene;
        MCP_CHECK(m_pActiveScene);

        // Load the Scene
        //MCP_LOG("SceneManager", "Loading Scene: ", m_sceneToTransitionTo.GetCStr());
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

        return true;
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

