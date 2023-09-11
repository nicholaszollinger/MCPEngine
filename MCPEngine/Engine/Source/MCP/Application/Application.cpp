// Application.cpp
#include "Application.h"

#include "BleachNew.h"
#include "Debug/Log.h"
#include "MCP/Audio/AudioManager.h"
#include "MCP/Collision/ColliderFactory.h"
#include "MCP/Components/ComponentFactory.h"
#include "MCP/Events/KeyEvent.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Lua/Lua.h"
#include "MCP/Scene/SceneManager.h"
#include "Resources/ResourceManager.h"
#include "utility/Time/FrameTimer.h"
#include "Window/WindowBase.h"

#ifdef MCP_DATA_PARSER_TINYXML2
#include "Platform/TinyXML2/tinyxml2.h"
#else
#error "Application doesn't have support for selected Data Parser!'"
#endif

namespace mcp
{
    void Application::Create()
    {
        if (!s_pInstance)
        {
            s_pInstance = BLEACH_NEW(Application);
        }
    }

    void Application::Destroy()
    {
        BLEACH_DELETE(s_pInstance);
        s_pInstance = nullptr;
    }

    Application::Application()
        : m_isRunning(false)
    {
        //
    }
    
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      TODO: In the future, I need to set up some window bitmask pertaining to different properties such as
    //          - Fullscreen
    //          - Fullscreen Border-less
    //          - Other settings...
    //
    //      TODO: Both the Logger and Bleach lib need to be initialized outside of the Engine.
    //
    ///		@brief : Initialize the MCPEngine and all of its dependencies.
    ///     @param props : Application properties to set up the application with.
    ///     @param pGameDataFilepath : Path to the GameData that we are using to run the game.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Application::Init(const ApplicationProperties& props, const char* pGameDataFilepath)
    {
        // Initialize Bleach Leak Detector:
        BLEACH_INIT_LEAK_DETECTOR();

        // Create GlobalManagers and save a reference in processes.
        GlobalManagerFactory::Create<LoggerInternal::Logger>();
        GlobalManagerFactory::Create<lua::LuaLayer>();
        GlobalManagerFactory::Create<GraphicsManager>();
        GlobalManagerFactory::Create<AudioManager>();
        GlobalManagerFactory::Create<ResourceManager>();
        GlobalManagerFactory::Create<SceneManager>();

        m_processes.emplace_back(LoggerInternal::Logger::Get());
        m_processes.emplace_back(GraphicsManager::Get());
        m_processes.emplace_back(AudioManager::Get());
        m_processes.emplace_back(ResourceManager::Get());
        m_processes.emplace_back(SceneManager::Get());
        m_processes.emplace_back(lua::LuaLayer::Get());

        // Initialize each process.
        for (auto* pProcess : m_processes)
        {
            // If any fail, then we need to close and quit.
            if (!pProcess->Init())
            {
                LogError("Failed to Initialize Application!");
                Close();
                return false;
            }
        }

        // Register Collider and Component Types:
        ColliderFactory::RegisterEngineColliderTypes();
        ComponentFactory::RegisterEngineComponentTypes();

        // Initialize the Window:
        if (!GraphicsManager::Get()->GetWindow()->Init(props.pWindowName, props.defaultWindowWidth, props.defaultWindowHeight))
        {
            LogError("Failed to initialize Application! Failed to initialize the Window!");
            Close();
            return false;
        }

        // Set the Window as the RenderTarget for the Renderer.
        if (!GraphicsManager::Get()->SetRenderTarget())
        {
            LogError("Failed to initialize Application! Failed to set the main window as the Render Target!");
            Close();
            return false;
        }

        // Load the GameData:
        if (!LoadGameData(pGameDataFilepath))
        {
            LogError("Failed to load the GameData at filepath: '%'", pGameDataFilepath);
            Close();
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Main Loop of the Application.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Application::Run()
    {
        m_isRunning = true;

        // Start the timer.
        FrameTimer timer;
        timer.Init();

        auto* pWindow = GraphicsManager::Get()->GetWindow();

        while (m_isRunning)
        {
            const float deltaTime = timer.NewFrame();

            m_isRunning = pWindow->ProcessEvents();

            if (m_isRunning)
            {
                SceneManager::Get()->Update(deltaTime);
                SceneManager::Get()->Render();
                GraphicsManager::Get()->Display();
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Loads the GameData necessary to run the game.
    ///		@param pGameDataFilepath : Path to the xml file on disk.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Application::LoadGameData(const char* pGameDataFilepath)
    {
#ifdef MCP_DATA_PARSER_TINYXML2
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(pGameDataFilepath) != tinyxml2::XML_SUCCESS)
        {
            LogError("Failed to load game data from file '%'", pGameDataFilepath);
            return false;
        }

        // Get the Scenes
        const auto* pSceneList = doc.FirstChildElement("SceneList");
        if (!pSceneList)
        {
            LogError("Failed to load game data! Couldn't find SceneList element!");
            return false;
        }

        const auto* pDefaultScene = pSceneList->FirstChildElement("Default");
        if (!pDefaultScene)
        {
            LogError("Failed to load game data! Couldn't find Default Scene element!");
            return false;
        }

        const char* pDefaultSceneFilepath = pDefaultScene->Attribute("sceneDataPath");
        if (!pDefaultSceneFilepath)
        {
            LogError("Failed to load game data! Couldn't find Default Scene Path attribute!");
            return false;
        }

        if (!SceneManager::Get()->LoadSceneData(pDefaultSceneFilepath))
        {
            LogError("Failed to load the initial Scene!");
            return false;
        }

        return true;
#else
        return false;
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Closes the Engine, shutting down all systems.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Application::Close() const
    {
        Log("Closing MCPEngine...");

        // Close the processes in reverse order.
        for (auto it = m_processes.rbegin(); it != m_processes.rend(); ++it)
        {
            (*it)->Close();
        }

        // Destroy all of the GlobalManagers:
        GlobalManagerFactory::Destroy<SceneManager>();
        GlobalManagerFactory::Destroy<ResourceManager>();
        GlobalManagerFactory::Destroy<AudioManager>();
        GlobalManagerFactory::Destroy<GraphicsManager>();
        GlobalManagerFactory::Destroy<lua::LuaLayer>();
        GlobalManagerFactory::Destroy<LoggerInternal::Logger>();
        BLEACH_DUMP_AND_DESTROY_LEAK_DETECTOR();
    }
}