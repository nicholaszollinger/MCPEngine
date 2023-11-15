#pragma once
// Application.h

#include <string>
#include <unordered_set>
#include <vector>
#include "MCP/Core/System.h"

struct lua_State;
namespace mcp
{
    class KeyEvent;
    class Scene;

    class Application
    {
        static inline Application* s_pInstance = nullptr;

        std::vector<System*> m_systems;
        bool m_isRunning;

    public:
        static void Create();
        static Application* Get() { return s_pInstance; }
        static void Destroy();

    public:
        Application();
        ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;
        
        bool Init(const char* pGameSystemsFilepath);
        void Run();
        void Quit();

        template<typename SystemType>
        SystemType* GetSystem() const;

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
