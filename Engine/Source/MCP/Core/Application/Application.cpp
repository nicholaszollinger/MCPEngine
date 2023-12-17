// Application.cpp
#include "Application.h"

#include <filesystem>
#include <fstream>
#include "BleachNew.h"
#include "MCP/Debug/Log.h"
#include "MCP/Audio/AudioManager.h"
#include "MCP/Collision/ColliderFactory.h"
#include "MCP/Components/ComponentFactory.h"
#include "MCP/Components/EngineComponents.h"
#include "MCP/Core/Event/ApplicationEvent.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Lua/Lua.h"
#include "LuaSource.h"
#include "MCP/Accessibility/Localization.h"
#include "MCP/Scene/SceneManager.h"
#include "MCP/Core/Resource/ResourceManager.h"
#include "Utility/Time/FrameTimer.h"
#include "Window/WindowBase.h"

namespace mcp
{
    void Application::Create(const ApplicationContext& context)
    {
        if (!s_pInstance)
        {
            s_pInstance = BLEACH_NEW(Application(context));
        }
    }

    void Application::Destroy()
    {
        BLEACH_DELETE(s_pInstance);
        s_pInstance = nullptr;
    }

    Application::Application(const ApplicationContext& context)
        : m_context(context)
        , m_isRunning(false)
    {
#if MCP_EDITOR
        // If the working directory is empty, set it to the current folder.
        if (m_context.workingDirectory.empty())
        {
            if (m_context.args.count == 1)
            {
                m_context.workingDirectory = std::filesystem::current_path().string() + "\\";
            }

            // If we have been sent a path in the executable, std::filesystem::current_path().string() will be incorrect.
            else
            {
                const std::string executablePath = m_context.args[0];
                const auto lastSlash = executablePath.find_last_of('\\');
                m_context.workingDirectory = executablePath.substr(0, lastSlash + 1);
            }

            MCP_LOG("Application", "Working Directory: ", m_context.workingDirectory);

        }
#else
        // If the working directory is empty, set it to the current folder.
        if (m_context.workingDirectory.empty())
        {
            m_context.workingDirectory = std::filesystem::current_path().string() + "\\";
        }
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Initialize the MCPEngine and all of its dependencies.
    ///		@param pGameSystemsFilepath : Path to the GameData that we are using to run the game.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Application::Init(const char* pGameSystemsFilepath)
    {
        // Initialize Bleach Leak Detector:
        //BLEACH_INIT_LEAK_DETECTOR();

#if MCP_LOGGING_ENABLED
        // Initialize the Logger
        if (!Logger::Init((m_context.workingDirectory + std::string("Log\\")).c_str()))
        {
            Close();
            return false;
        }

        // Load the Engine log categories.
        if (!Logger::LoadCategories((m_context.workingDirectory + std::string(R"(Config\Engine\Logs.config)")).c_str()))
        {
            MCP_ERROR("Application", "Failed to load Engine Log Categories!");
            Close();
            return false;
        }

        // Load the Game log categories.
        if (!Logger::LoadCategories((m_context.workingDirectory + std::string(R"(Config\GameLogs.config)")).c_str()))
        {
            MCP_ERROR("Application", "Failed to load Game log categories!");
            Close();
            return false;
        }

#endif

        // Create the Resource system immediately and initialize it:
        auto* pResourceManager = m_systems.emplace_back(BLEACH_NEW(ResourceManager));
        if (!pResourceManager->Init())
        {
            MCP_ERROR("application", "Failed to initialize! Failed to initialize the ResourceManager!");
            Close();
            return false;
        }

        // Opening a block to control the lifetime of the project settings file.
        {
            // Load the project settings:
            XMLParser projectSettingsFile;
            // TODO: This file path could be a parameter:
            if (!projectSettingsFile.LoadFile("Config\\ProjectSettings.xml"))
            {
                MCP_CRITICAL("Application", "Failed to load project settings!");
                Close();
                return false;
            }

            const auto settingsRoot = projectSettingsFile.GetElement();

            // Add the Engine Systems using the project settings:
            m_systems.emplace_back(LocalizationSystem::AddFromData(settingsRoot));
            m_systems.emplace_back(lua::LuaSystem::AddFromData(settingsRoot));
            m_systems.emplace_back(GraphicsManager::AddFromData(settingsRoot));
            m_systems.emplace_back(AudioManager::AddFromData(settingsRoot));
            m_systems.emplace_back(SceneManager::AddFromData(settingsRoot));
        }

        // Initialize each engine system in order. (skipping the ResourceManager)
        for (size_t i = 1; i < m_systems.size(); ++i)
        {
            // If any fail, then we need to close and quit.
            if (!m_systems[i]->Init())
            {
                MCP_ERROR("Application", "Failed to Initialize Application! Failed to initialize Engine Systems!");
                Close();
                return false;
            }
        }

        const auto gameSystemIndex = m_systems.size();

        // Load the GameSystems:
        if (!LoadGameSystems(pGameSystemsFilepath))
        {
            MCP_ERROR("Application", "Failed to load GameSystems!");
            Close();
            return false;
        }

        // Initialize Game Systems
        for (size_t i = gameSystemIndex; i < m_systems.size(); ++i)
        {
            // If any fail, then we need to close and quit.
            if (!m_systems[i]->Init())
            {
                MCP_ERROR("Application", "Failed to Initialize Application! Failed to initialize Game Systems!");
                Close();
                return false;
            }
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
        // Enter the start scene:
        if (!SceneManager::Get()->EnterStartScene())
            return;

        m_isRunning = true;

        // Start the timer.
        FrameTimer timer;

        auto* pWindow = GraphicsManager::Get()->GetWindow();

        while (m_isRunning)
        {
            [[maybe_unused]] const float deltaTimeMs = static_cast<float>(timer.NewFrame());

            m_isRunning = pWindow->ProcessEvents();

            if (m_isRunning)
            {
#ifndef MCP_EDITOR
                SceneManager::Get()->Update(deltaTimeMs);
#endif
                SceneManager::Get()->Render();
                GraphicsManager::Get()->Display();
            }
        }

        // Safely close the Application.
        Close();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Attempts to load each game system. If any GameSystem fails to load properly, this will return false.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Application::LoadGameSystems(const char* pGameSystemsPath)
    {
        // Load GameSystems:
        XMLParser gameSystems;
        if (gameSystems.LoadFile(pGameSystemsPath))
        {
            auto systemElement = gameSystems.GetElement("System");
            XMLParser gameSystemData;

            while(systemElement.IsValid())
            {
                // Get the System typename
                auto* pSystemType = systemElement.GetAttributeValue<const char*>("typename", nullptr);
                if (!pSystemType)
                {
                    MCP_ERROR("Application", "Failed to load game system! System 'typename' was invalid!");
                    systemElement = systemElement.GetSiblingElement("System");
                    continue;
                }

                // Get the Path to the data to construct the System
                auto* pDataPath = systemElement.GetAttributeValue<const char*>("dataPath", nullptr);

                // If there is a valid path:
                if (pDataPath)
                {
                    if (!gameSystemData.LoadFile(pDataPath))
                    {
                        MCP_ERROR("Application", "Failed to load game system: ", pSystemType, "! 'dataPath' was invalid!");
                        return false;
                    }

                    const auto rootElement = gameSystemData.GetElement();
                    auto* pSystem = SystemFactory::CreateTypeFromData(pSystemType, rootElement);

                    if (!pSystem)
                    {
                        MCP_ERROR("Application", "Failed to load game system: ", pSystemType, " from data!");
                        return false;
                    }

                    m_systems.emplace_back(pSystem);

                    // Close the current file we were working on.
                    gameSystemData.CloseCurrentFile();
                }

                // If there is no data path, try to create a new default system:
                else
                {
                    auto* pSystem = SystemFactory::CreateDefaultType(pSystemType);

                    if (!pSystem)
                    {
                        MCP_ERROR("Application", "Failed to create game system: ", pSystemType, "!");
                        return false;
                    }

                    m_systems.emplace_back(pSystem);
                }

                // Continue to the next system:
                systemElement = systemElement.GetSiblingElement("System");
            }
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Quit the Application.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Application::Quit()
    {
        ApplicationQuitEvent event;
        GraphicsManager::Get()->GetWindow()->PostApplicationEvent(event);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Closes the Engine, shutting down all systems.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Application::Close()
    {
        if (m_isRunning)
            return;

        MCP_LOG("Application", "Closing MCPEngine...");

        // Close the Systems in reverse order.
        for (auto it = m_systems.rbegin(); it != m_systems.rend(); ++it)
        {
            auto* pSystem = (*it);
            pSystem->Close();

            BLEACH_DELETE(pSystem);
            pSystem = nullptr;
        }

#if MCP_LOGGING_ENABLED
        // Close the Logger
        Logger::Close();
#endif

        //BLEACH_DUMP_AND_DESTROY_LEAK_DETECTOR();
    }

    static int QuitGame([[maybe_unused]] lua_State* pState)
    {
        Application::Get()->Quit();
        return 0;
    }

    static int SetLanguage(lua_State* pState)
    {
        const char* pLanguageToken = lua_tostring(pState, -1);
        lua_pop(pState, 1);

        LocalizationSystem::Get()->SetLanguage(pLanguageToken);

        return 0;
    }

    void Application::RegisterLuaFunctions(lua_State* pState)
    {
        static constexpr luaL_Reg kFuncs[]
        {
            {"QuitGame", QuitGame }
            ,{"SetLanguage", SetLanguage }
            ,{nullptr, nullptr}
        };

        // Get the global Application library class,
        lua_getglobal(pState, "Application");
        MCP_CHECK(lua_type(pState, -1) == LUA_TTABLE);

        // Set its functions
        luaL_setfuncs(pState, kFuncs, 0);

        // Pop the table off the stack.
        lua_pop(pState, 1);

    }
}