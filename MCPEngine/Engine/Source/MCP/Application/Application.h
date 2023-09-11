#pragma once
// Application.h

#include <vector>

namespace mcp
{
    struct KeyEvent;
    class Scene;
    class IProcess;
    //class LuaSystem;

    struct ApplicationProperties
    {
        // Name to give the Application's Main Window.
        const char* pWindowName  = nullptr;

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
        
        bool Init(const ApplicationProperties& props, const char* pGameDataFilepath);
        void Run();
        void Close() const;

    private:
        bool LoadGameData(const char* pGameDataFilepath);
    };


}
