// SDL2Window.cpp

#include "SDL2Window.h"

#pragma warning(push)
#pragma warning(disable : 26819)
#include <SDL_events.h>
#include <SDL_render.h>
#pragma warning(pop)

#include "SDLHelpers.h"
#include "MCP/Core/Event/ApplicationEvent.h"
#include "MCP/Core/Event/Event.h"
#include "MCP/Debug/Log.h"
#include "MCP/Graphics/Graphics.h"

namespace mcp
{
    SDL2Window::SDL2Window()
        : m_pWindow(nullptr)
        , m_pRenderer(nullptr)
        , m_dimensions{}
    {
        //
    }

    SDL2Window::~SDL2Window()
    {
        Close();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Flags for SDL_Window: https://wiki.libsdl.org/SDL2/SDL_WindowFlags
    //
    ///		@brief : Create a new Window centered in the screen.
    ///		@returns : False if initialization fails, otherwise true.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool SDL2Window::Init(const char* pWindowName, const int width, const int height)
    {
        m_dimensions.SetPosition(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

        Uint32 flags{};

        if(width == -1 || height == -1)
        {
           flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }

        // Create the Window.
        m_pWindow = SDL_CreateWindow(pWindowName
            , m_dimensions.x
            , m_dimensions.y
            , width
            , height
            , flags);

        if (!m_pWindow)
        {
            MCP_ERROR("SDLWindow", "Failed to create SDL_Window! SDL_Error: ", SDL_GetError());
            return false;
        }

        SDL_GetWindowSize(m_pWindow, &m_dimensions.width, &m_dimensions.height);

        // Create the Renderer:
        m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);

        if (!m_pRenderer)
        {
            MCP_ERROR("SDLWindow", "Failed to create SDL_Renderer! SDL_Error: ", SDL_GetError());
            SDL_DestroyWindow(m_pWindow);
            return false;
        }

        // Set the blend mode to allow alpha blending:
        if (SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND) < 0)
        {
            MCP_ERROR("SDLWindow", "Failed to Set the BlendMode of the Renderer! SDL_Error: ", SDL_GetError());
            SDL_DestroyRenderer(m_pRenderer);
            SDL_DestroyWindow(m_pWindow);
            return false;
        }

        return true;
    }

    bool SDL2Window::ProcessEvents()
    {
        SDL_Event sdlEvent;

        auto dispatchFunc = GlobalEventDispatcher::InvokeNow<ApplicationEvent>;

        while (SDL_PollEvent(&sdlEvent) != 0)
        {
            switch(sdlEvent.type)
            {
                case SDL_QUIT: return false;

                // TODO: Define what to do for Window-based events.

                case SDL_KEYDOWN:
                {
                    const MCPKey keycode = KeyToSDL(sdlEvent.key.keysym.scancode);
                    const bool ctrl = sdlEvent.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL);
                    const bool shift = sdlEvent.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT);
                    const bool alt = sdlEvent.key.keysym.mod & (KMOD_LALT | KMOD_RALT);
                    ButtonState state;
                    if (sdlEvent.key.repeat != 0)
                        state = ButtonState::kHeld;

                    else
                        state = ButtonState::kPressed;
                    
                    KeyEvent keyEvent(keycode, state, ctrl, alt, shift);

                    // Dispatch the event.
                    dispatchFunc(keyEvent);

                    break;
                }

                case SDL_KEYUP:
                {
                    const MCPKey keycode = KeyToSDL(sdlEvent.key.keysym.scancode);
                    const bool ctrl = sdlEvent.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL);
                    const bool shift = sdlEvent.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT);
                    const bool alt = sdlEvent.key.keysym.mod & (KMOD_LALT | KMOD_RALT);
                    const ButtonState state = ButtonState::kReleased;

                    KeyEvent keyEvent(keycode, state, ctrl, alt, shift);
                    // Dispatch the event.
                    dispatchFunc(keyEvent);
                    break;
                }

                case SDL_MOUSEMOTION:
                {
                    const auto x = static_cast<float>(sdlEvent.motion.x);
                    const auto y = static_cast<float>(sdlEvent.motion.y);

                    MouseMoveEvent mouseMoveEvent(x, y);
                    dispatchFunc(mouseMoveEvent);
                    break;
                }

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                {
                    const ButtonState state = sdlEvent.button.type == SDL_MOUSEBUTTONDOWN ? ButtonState::kPressed : ButtonState::kReleased;
                    const auto x = static_cast<float>(sdlEvent.button.x);
                    const auto y = static_cast<float>(sdlEvent.button.y);
                    const auto button = ToMouseButton(sdlEvent.button.button);
                    const int clicks = sdlEvent.button.clicks;
                    MouseButtonEvent buttonEvent(button, state, clicks, x, y);
                    dispatchFunc(buttonEvent);
                    break;
                }

                case SDL_MOUSEWHEEL:
                {
                    MouseScrolledEvent scrolledEvent(sdlEvent.wheel.x, sdlEvent.wheel.y);
                    dispatchFunc(scrolledEvent);
                    break;
                }

                // TODO: Window Events.
                // TODO: Controller Events.

                default: break;
            }
        }

        return true;
    }

    Vec2 SDL2Window::GetMousePosition() const
    {
        int x = 0;
        int y = 0;
        SDL_GetMouseState(&x, &y);

        return {static_cast<float>(x), static_cast<float>(y)};
    }

    void SDL2Window::PostApplicationEvent(ApplicationEvent& event)
    {
        const auto eventId = event.GetEventId();

        if (eventId == ApplicationQuitEvent::GetStaticId())
        {
            SDL_Event quit;
            quit.type = SDL_QUIT;
            SDL_PushEvent(&quit);
            event.SetHandled();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Safely close the window.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SDL2Window::Close() const
    {
        SDL_DestroyRenderer(m_pRenderer);
        SDL_DestroyWindow(m_pWindow);
    }
}