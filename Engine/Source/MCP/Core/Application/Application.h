#pragma once
// Application.h

#include <string>
#include <vector>
#include "GameInstance.h"

struct lua_State;
namespace mcp
{
    class KeyEvent;
    class Scene;
    class IProcess;

    struct ApplicationProperties
    {
        // Name to give the Application's Main Window.
        std::string windowName;

        // Starting window width of the application. Setting either width or height to -1
        // will default to fullscreen.
        int defaultWindowWidth   = -1;

        // Starting window height of the application. Setting either width or height to -1
        // will default to fullscreen.
        int defaultWindowHeight  = -1;
    };

    class Application
    {
        static inline Application* s_pInstance = nullptr;

        std::vector<IProcess*> m_processes;
        GameInstance* m_pGameInstance = nullptr;
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
        
        bool Init(const char* pGameDataFilepath, GameInstance* pGameInstance);
        void Run();
        void Quit();

        [[nodiscard]] const GameInstance* GetGameInstance() const { return m_pGameInstance; }

        static void RegisterLuaFunctions(lua_State* pState);

    private:
        bool LoadApplicationProperties(ApplicationProperties& outProps, const char* pFilepath);
        void Close();
    };

}
