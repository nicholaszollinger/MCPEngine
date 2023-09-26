// Application.cpp
#include "Application.h"

#include <fstream>
#include "BleachNew.h"
#include "MCP/Debug/Log.h"
#include "MCP/Audio/AudioManager.h"
#include "MCP/Collision/ColliderFactory.h"
#include "MCP/Components/ComponentFactory.h"
#include "MCP/Components/EngineComponents.h"
#include "MCP/Core/Resource/Parser.h"
#include "MCP/Core/Event/KeyEvent.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Lua/Lua.h"
#include "MCP/Scene/SceneManager.h"
#include "MCP/Core/Resource/ResourceManager.h"
#include "Utility/Time/FrameTimer.h"
#include "Window/WindowBase.h"

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

#if MCP_LOGGING_ENABLED
        // Initialize the Logger
        if (!Logger::Init("Log/"))
        {
            Close();
            return false;
        }

        // Load the Engine log categories.
        if (!Logger::LoadCategories("../MCPEngine/Engine/Config/EngineLogs.config"))
        {
            MCP_ERROR("Application", "Failed to load EngineLog Categories!");
            Close();
            return false;
        }
#endif

        // Create GlobalManagers and save a reference in processes.
        GlobalManagerFactory::Create<lua::LuaLayer>();
        GlobalManagerFactory::Create<GraphicsManager>();
        GlobalManagerFactory::Create<AudioManager>();
        GlobalManagerFactory::Create<ResourceManager>();
        GlobalManagerFactory::Create<SceneManager>();
        
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
                MCP_ERROR("Application", "Failed to Initialize Application!");
                Close();
                return false;
            }
        }

        // Initialize the Window:
        if (!GraphicsManager::Get()->GetWindow()->Init(props.windowName.c_str(), props.defaultWindowWidth, props.defaultWindowHeight))
        {
            MCP_ERROR("Application", "Failed to initialize Application! Failed to initialize the Window!");
            Close();
            return false;
        }

        // Set the Window as the RenderTarget for the Renderer.
        if (!GraphicsManager::Get()->SetRenderTarget())
        {
            MCP_ERROR("Application", "Failed to initialize Application! Failed to set the main window as the Render Target!");
            Close();
            return false;
        }

        // Load the GameData:
        if (!LoadGameData(pGameDataFilepath))
        {
            MCP_ERROR("Application", "Failed to load the GameData at filepath: ", pGameDataFilepath);
            Close();
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Initialize the MCPEngine and all of its dependencies.
    ///		@param pGameDataFilepath : Path to the GameData that we are using to run the game.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Application::Init(const char* pGameDataFilepath)
    {
        // Initialize Bleach Leak Detector:
        BLEACH_INIT_LEAK_DETECTOR();

#if MCP_LOGGING_ENABLED
        // Initialize the Logger
        if (!Logger::Init("Log/"))
        {
            Close();
            return false;
        }

        // Load the Engine log categories.
        if (!Logger::LoadCategories("../MCPEngine/Engine/Config/EngineLogs.config"))
        {
            MCP_ERROR("Application", "Failed to load EngineLog Categories!");
            Close();
            return false;
        }

        // Load the Game log categories.
        if (!Logger::LoadCategories("Config/GameLogs.config"))
        {
            MCP_ERROR("Application", "Failed to load Game log categories!");
            Close();
            return false;
        }

#endif

        // Load the ApplicationProperties from Data.
        ApplicationProperties props;
        if (!LoadApplicationProperties(props, "Config/AppProps.config"))
        {
            MCP_CRITICAL("Application", "Failed to load ApplicationProperties!");
            Close();
            return false;
        }

        // Create GlobalManagers and save a reference in processes.
        GlobalManagerFactory::Create<lua::LuaLayer>();
        GlobalManagerFactory::Create<GraphicsManager>();
        GlobalManagerFactory::Create<AudioManager>();
        GlobalManagerFactory::Create<ResourceManager>();
        GlobalManagerFactory::Create<SceneManager>();
        
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
                MCP_ERROR("Application", "Failed to Initialize Application!");
                Close();
                return false;
            }
        }

        // Initialize the Window:
        if (!GraphicsManager::Get()->GetWindow()->Init(props.windowName.c_str(), props.defaultWindowWidth, props.defaultWindowHeight))
        {
            MCP_ERROR("Application", "Failed to initialize Application! Failed to initialize the Window!");
            Close();
            return false;
        }

        // Set the Window as the RenderTarget for the Renderer.
        if (!GraphicsManager::Get()->SetRenderTarget())
        {
            MCP_ERROR("Application", "Failed to initialize Application! Failed to set the main window as the Render Target!");
            Close();
            return false;
        }

        // Load the GameData:
        if (!LoadGameData(pGameDataFilepath))
        {
            MCP_ERROR("Application", "Failed to load the GameData at filepath: ", pGameDataFilepath);
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

        auto* pWindow = GraphicsManager::Get()->GetWindow();

        while (m_isRunning)
        {
            const float deltaTimeMs = static_cast<float>(timer.NewFrame());

            m_isRunning = pWindow->ProcessEvents();

            if (m_isRunning)
            {
                SceneManager::Get()->Update(deltaTimeMs);
                SceneManager::Get()->Render();
                GraphicsManager::Get()->Display();
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      This is very 'built for specific data'. I would like to extract certain functions when I get to making my parser interfaces.
    //		
    ///		@brief : 
    ///		@param outProps : 
    ///		@param pFilepath : 
    ///		@returns : 
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Application::LoadApplicationProperties(ApplicationProperties& outProps, const char* pFilepath)
    {
        std::ifstream inFile(pFilepath, std::ios::in);
        if (!inFile.is_open())
        {
            MCP_ERROR("Application", "Failed to open AppProps.config at filepath: ", pFilepath);
            return false;
        }

        // Lambda to check for a whitespace char.
        const auto isWhiteSpace = [](const char val) -> bool
        {
           return val == ' '
                || val == '\t'
                || val == '\n'
                || val == '\r'
                || val == '\v'
                || val == '\f';
        };

        // Lambda to remove all whitespace from the line.
        // I can't use the c++20 version -> std::erase_if(line, isWhiteSpace);
        // So I just grabbed the implementation from cppreference and put it in a lambda.
        // https://en.cppreference.com/w/cpp/string/basic_string/erase2
        auto eraseIf = [](std::string& str, const std::function<bool(const char)>& predicate) -> void
        {
            const auto it = std::remove_if(str.begin(), str.end(), predicate);
            str.erase(it, str.end());
        };

        std::string line;

        // Go to the first item.
        while (std::getline(inFile, line))
        {
            if (line[0] != '#' && line[0] != ';' && !line.empty())
                break;
        }

        // Get the Window Name.
        auto endOfName = line.find_first_of('=');
        outProps.windowName = line.substr(endOfName + 1, std::string::npos);

        // Get the Width
        std::getline(inFile, line);
        endOfName = line.find_first_of('=');
        auto val = line.substr(endOfName + 1, std::string::npos);
        outProps.defaultWindowWidth = std::stoi(val);

        // Get the Height
        std::getline(inFile, line);
        endOfName = line.find_first_of('=');
        val = line.substr(endOfName + 1, std::string::npos);
        outProps.defaultWindowHeight = std::stoi(val);

        return true;
    }


    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Loads the GameData necessary to run the game.
    ///		@param pGameDataFilepath : Path to the xml file on disk.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Application::LoadGameData(const char* pGameDataFilepath)
    {
        XMLParser parser;
        
        if (!parser.LoadFile(pGameDataFilepath))
        {
            MCP_ERROR("Application", "Failed to load game data from file: ", pGameDataFilepath);
            return false;
        }

        // Get the Scenes
        const XMLElement sceneList = parser.GetElement("SceneList");
        if (!sceneList.IsValid())
        {
            MCP_ERROR("Application", "Failed to load game data! Couldn't find SceneList element!");
            return false;
        }

        const XMLElement defaultScene = sceneList.GetChildElement("Default");
        if (!defaultScene.IsValid())
        {
            MCP_ERROR("Application", "Failed to load game data! Couldn't find Default Scene element!");
            return false;
        }

        const char* pDefaultSceneFilepath = defaultScene.GetAttribute<const char*>("sceneDataPath");
        if (!pDefaultSceneFilepath)
        {
            MCP_ERROR("Application", "Failed to load game data! Couldn't find Default Scene Path attribute!");
            return false;
        }

        if (!SceneManager::Get()->LoadSceneData(pDefaultSceneFilepath))
        {
            MCP_ERROR("Application", "Failed to load the initial Scene!");
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Closes the Engine, shutting down all systems.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Application::Close() const
    {
        MCP_LOG("Application", "Closing MCPEngine...");

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

#if MCP_LOGGING_ENABLED
        Logger::Close();
#endif

        BLEACH_DUMP_AND_DESTROY_LEAK_DETECTOR();
    }
}