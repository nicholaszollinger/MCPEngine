#pragma once
// Application.h

#include <string>
#include <unordered_set>
#include <vector>
#include "MCP/Core/System.h"
#include "MCP/Core/Event/ApplicationEvent.h"
#include "MCP/Graphics/Graphics.h"

struct lua_State;

namespace mcp
{
    class KeyEvent;
    class Scene;

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Container for the arguments passed into the executable. The first argument will always be the executable's path.
    //-----------------------------------------------------------------------------------------------------------------------------
    struct CommandLineArgs
    {
	    size_t count = 0;
	    char** args = nullptr;

	    const char* operator[](const size_t index) const
	    {
		    MCP_CHECK(index < count);
		    return args[index];
	    }

        const char* operator[](const int index) const
	    {
		    MCP_CHECK(static_cast<size_t>(index) < count);
		    return args[index];
	    }
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Data used to create this instance of the Application.
    //-----------------------------------------------------------------------------------------------------------------------------
    struct ApplicationContext
    {
        //ApplicationContext(CommandLineArgs&& args, std::string&& workingDirectory);
        
        // The arguments passed into the Application's executable
        CommandLineArgs args;

        // The root folder for relative asset search paths.
        std::string workingDirectory;
    };

    class Application
    {
        static inline Application* s_pInstance = nullptr;

        std::vector<System*> m_systems;
        ApplicationContext m_context;
        bool m_isRunning;

    public:
        static void Create(const ApplicationContext& context);
        static Application* Get() { return s_pInstance; }
        static void Destroy();

    private:
        Application(const ApplicationContext& context);
        ~Application() = default;

    public:
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        bool Init(const char* pGameSystemsFilepath);
        void Run();
        void Quit();

        template<typename SystemType>
        SystemType* GetSystem() const;

        [[nodiscard]] const ApplicationContext& GetContext() const { return m_context; }

        static void RegisterLuaFunctions(lua_State* pState);

    private:
        bool LoadGameSystems(const char* pGameSystemsPath);
        void Close();
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get one of the global Systems in our Application.
    ///		@tparam SystemType : Type of system you are looking for. Example 'GraphicsSystem'.
    ///		@returns : Pointer to the system, or nullptr if the system was not found.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename SystemType>
    SystemType* Application::GetSystem() const
    {
        static_assert(std::is_base_of_v<System, SystemType>, "SystemType must be a derived class of System!");

        for (auto* pSystem : m_systems)
        {
            if (pSystem->GetTypeId() == SystemType::GetStaticTypeId())
            {
                // TODO: This should be checked_cast
                return static_cast<SystemType*>(pSystem);
            }
        }

        return nullptr;
    }
}
