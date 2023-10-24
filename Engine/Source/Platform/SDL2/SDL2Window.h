#pragma once
// SDL2Window.h

#include "Utility/Types/Rect.h"

struct SDL_Window;
struct SDL_Renderer;

namespace mcp
{
    class ApplicationEvent;
    class Event;

    class SDL2Window
    {
        SDL_Window* m_pWindow;      // Pointer to the window instance.
        SDL_Renderer* m_pRenderer;  // Pointer to the renderer for this window.
        RectInt m_dimensions;       // Position and size data for this window.

    public:
        SDL2Window();
        ~SDL2Window();
        
        bool Init(const char* pWindowName, const int width, const int height);

        bool ProcessEvents();

        void PostApplicationEvent(ApplicationEvent& event);
        void Close() const;

        [[nodiscard]] Vec2 GetMousePosition() const;
        [[nodiscard]] const RectInt& GetDimensions() const { return m_dimensions; }
        [[nodiscard]] SDL_Renderer* GetRenderer() const { return m_pRenderer; }
    };
}