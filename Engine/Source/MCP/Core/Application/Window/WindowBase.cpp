// WindowBase.cpp

#include "WindowBase.h"

#include "MCP/Core/Config.h"

#if MCP_WINDOW_PLATFORM == MCP_WINDOW_PLATFORM_SDL
#include "Platform/SDL2/SDL2Window.h"

namespace mcp
{
    using WindowType = SDL2Window;
}

#else
#error "MPC Engine does not support WindowBase creation for this Render Library!"
#endif

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Set up WindowBase with initial data and open it. 
    ///		@param pWindowName : Name of the window.
    ///		@param width : Initial width of the window.
    ///		@param height : Initial height of the window.
    ///		@returns : False if something went wrong in the initialization.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool WindowBase::Init(const char* pWindowName, const int width, const int height)
    {
        return GetWindow<WindowType>().Init(pWindowName, width, height);
    }

    const RectInt& WindowBase::GetDimensions()
    {
        return GetWindow<WindowType>().GetDimensions();
    }

    void* WindowBase::GetRenderer()
    {
        return GetWindow<WindowType>().GetRenderer();
    }

    bool WindowBase::ProcessEvents()
    {
        return GetWindow<WindowType>().ProcessEvents();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Closes the window.
    //-----------------------------------------------------------------------------------------------------------------------------
    void WindowBase::Close()
    {
        GetWindow<WindowType>().Close();
    }
}