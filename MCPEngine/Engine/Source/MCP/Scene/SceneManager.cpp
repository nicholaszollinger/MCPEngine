// SceneManager.cpp

#include "SceneManager.h"
#include <BleachNew.h>

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
        // TEMPORARY SOLUTION:
        m_pActiveScene->Unload();
        BLEACH_DELETE(m_pActiveScene);
    }

    bool SceneManager::LoadSceneData(const char* pSceneFilePath)
    {
        // Load the default scene:
        m_pActiveScene = BLEACH_NEW(Scene);

        if (!m_pActiveScene->Load(pSceneFilePath))
        {
            Close();
            return false;
        }

        if (!m_pActiveScene->OnSceneLoad())
        {
            Close();
            return false;
        }

        return true;
    }

    void SceneManager::Update(const float deltaTime)
    {
        if (m_pActiveScene)
        {
            m_pActiveScene->Update(deltaTime);

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
        m_transitionQueued = true;
        m_sceneToTransitionTo = identifier;
    }

    bool SceneManager::TransitionToScene()
    {
        // TODO: Some kind of check to make sure that the sceneIdentifier makes sense.

        if (m_pActiveScene)
        {
            m_pActiveScene->Unload(); // This isn't great, because I can't asynchronously transition between scenes.
        }

        // Use the Identifier to get the build data from memory.

        // TODO: Need to figure out how we are getting this value:
        if (m_pActiveScene->Load(""))
        {
            LogError("Failed to Load Scene =%", m_sceneToTransitionTo);
            return false;
        }

        if (!m_pActiveScene->OnSceneLoad())
        {
            
        }
        m_transitionQueued = false;
        // TODO: Should I invalidate m_sceneToTransitionTo?

        return true;
    }

}

