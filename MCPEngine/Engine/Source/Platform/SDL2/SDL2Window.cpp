// SDL2Window.cpp

#include "SDL2Window.h"

#pragma warning(push)
#pragma warning(disable : 26819)
#include <SDL_events.h>
#include <SDL_render.h>
#pragma warning(pop)

#include "SDLHelpers.h"
#include "MCP/Application/Debug/Log.h"
#include "MCP/Events/KeyEvent.h"
#include "MCP/Events/UserEvent.h"
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
        m_dimensions.position.x = SDL_WINDOWPOS_CENTERED;
        m_dimensions.position.y = SDL_WINDOWPOS_CENTERED;

        Uint32 flags{};

        if(width == -1 || height == -1)
        {
           flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }

        // Create the Window.
        m_pWindow = SDL_CreateWindow(pWindowName
            , m_dimensions.position.x
            , m_dimensions.position.x
            , width
            , height
            , flags);

        if (!m_pWindow)
        {
            LogError("Failed to create SDL_Window! SDL_Error: ", SDL_GetError());
            return false;
        }

        SDL_GetWindowSize(m_pWindow, &m_dimensions.width, &m_dimensions.height);

        // Create the Renderer:
        m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);

        if (!m_pRenderer)
        {
            LogError("Failed to create SDL_Renderer! SDL_Error: ", SDL_GetError());
            SDL_DestroyWindow(m_pWindow);
            return false;
        }

        // Set the blend mode to allow alpha blending:
        if (SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND) < 0)
        {
            LogError("Failed to Set the BlendMode of the Renderer! SDL_Error: ", SDL_GetError());
            SDL_DestroyRenderer(m_pRenderer);
            SDL_DestroyWindow(m_pWindow);
            return false;
        }

        return true;
    }

    bool SDL2Window::ProcessEvents()
    {
        SDL_Event sdlEvent;

        while (SDL_PollEvent(&sdlEvent) != 0)
        {
            switch(sdlEvent.type)
            {
                case SDL_QUIT: return false;

                // TODO: Define what to do for Window-based events.

            case SDL_KEYDOWN:
            {
                 KeyEvent keyEvent;
                 keyEvent.keycode = KeyToSDL(sdlEvent.key.keysym.scancode);
                 keyEvent.ctrl = sdlEvent.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL);
                 keyEvent.shift = sdlEvent.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT);
                 keyEvent.alt = sdlEvent.key.keysym.mod & (KMOD_LALT | KMOD_RALT);

                 if (sdlEvent.key.repeat != 0)
                 {
                     keyEvent.state = KeyState::kHeld;
                 }

                 else
                 {
                     keyEvent.state = KeyState::kPressed;
                 }

                 // Dispatch the event.
                 EventDispatcher::InvokeNow(keyEvent);

                 break;
            }

            case SDL_KEYUP:
            {
                KeyEvent keyEvent;
                keyEvent.keycode = KeyToSDL(sdlEvent.key.keysym.scancode);
                keyEvent.ctrl = sdlEvent.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL);
                keyEvent.shift = sdlEvent.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT);
                keyEvent.alt = sdlEvent.key.keysym.mod & (KMOD_LALT | KMOD_RALT);
                keyEvent.state = KeyState::kReleased;

                // Dispatch the event.
                EventDispatcher::InvokeNow(keyEvent);
                break;
            }

            default: break;
            }
        }

        return true;
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