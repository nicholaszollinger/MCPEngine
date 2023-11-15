// Graphics.cpp

#include "Graphics.h"

#include "MCP/Core/Config.h"
#include "MCP/Debug/Log.h"
#include "MCP/Core/Application/Window/WindowBase.h"
#include "MCP/Core/Application/Application.h"

#if MCP_RENDERER_API == MCP_RENDERER_API_SDL

    #pragma warning(push, 0)
    #include "Platform/SDL2/SDLRenderer.h"
    #pragma warning(pop)
    
    using Renderer = SdlRenderer;

#endif

namespace mcp
{
    MCP_DEFINE_STATIC_SYSTEM_GETTER(GraphicsManager)

    GraphicsManager::GraphicsManager(WindowConstructionData&& data)
        : m_mainWindowData(std::move(data))
    {
        //
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Initialize the renderer.
    ///		@returns : True if the initialization was successful.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool GraphicsManager::Init()
    {
        // Create the window class
        m_pWindow = BLEACH_NEW(WindowBase);

        // Initialize the Renderer with the Window class.
        if (!Renderer::Init())
        {
            MCP_ERROR("Renderer", "Failed to initialize GraphicsManager! Failed to initialize Renderer!");
            Close();
            return false;
        }

        // Create/Initialize the main window:
        if (!m_pWindow->Init(m_mainWindowData.windowName.c_str(), m_mainWindowData.dimensions.x, m_mainWindowData.dimensions.y))
        {
            MCP_ERROR("Renderer", "Failed to initialize GraphicsManager! Failed to initialize MainWindow!");
            Close();
            return false;
        }

        // Set the Render Target to the main window
        if (!SetRenderTarget(m_pWindow))
        {
            MCP_ERROR("Renderer", "Failed to initialize GraphicsManager! Failed to set the window as the RenderTarget!");
            Close();
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Closes the Renderer and destroys the Window.
    //-----------------------------------------------------------------------------------------------------------------------------
    void GraphicsManager::Close()
    {
        // Close and delete the Window.
        m_pWindow->Close();
        BLEACH_DELETE(m_pWindow);

        // Close the Renderer.
        Renderer::Close();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Get the renderer for this application.
    //-----------------------------------------------------------------------------------------------------------------------------
    void* GraphicsManager::GetRenderer() const
    {
        return m_pWindow->GetRenderer();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Displays the current Render information to the window.
    //-----------------------------------------------------------------------------------------------------------------------------
    void GraphicsManager::Display()
    {
        Renderer::Display();
    }

    bool GraphicsManager::SetRenderTarget(WindowBase* pTarget) const
    {
        if (!Renderer::SetRenderTarget(pTarget))
        {
            MCP_ERROR("Renderer", "Failed to SetRenderTarget!");
            return false;
        }

        return true;
    }

    GraphicsManager* GraphicsManager::AddFromData(const XMLElement element)
    {
        // Get the Window Data:
        const auto windowElement = element.GetChildElement("Window");
        if (!windowElement.IsValid())
        {
            MCP_CRITICAL("GraphicsManager", "Failed to create GraphicsManager from data! Failed to find Window Element!");
            return nullptr;
        }

        WindowConstructionData data;

        auto windowChildElement = windowElement.GetChildElement("Dimensions");
        if (windowElement.IsValid())
        {
            data.dimensions.x = windowChildElement.GetAttributeValue<int>("width", 1600);
            data.dimensions.y = windowChildElement.GetAttributeValue<int>("height", 900);
        }

        windowChildElement = windowElement.GetChildElement("Style");
        if (windowChildElement.IsValid())
        {
            data.windowName = windowChildElement.GetAttributeValue<const char*>("name", "Game");
        }

        return BLEACH_NEW(GraphicsManager(std::move(data)));
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Set the draw color for primitive draw calls.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SetDrawColor(const Color& color)
    {
        Renderer::SetDrawColor(color);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Fills the entire screen with a single color.
    //-----------------------------------------------------------------------------------------------------------------------------
    void FillScreen(const Color& color)
    {
        Renderer::FillScreen(color);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Draw a line between two points of a certain color.
    //-----------------------------------------------------------------------------------------------------------------------------
    void DrawLine(const Vec2Int& a, const Vec2Int& b, const Color& color)
    {
        Renderer::DrawLine(a, b, color);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Draw a filled rect of a certain color.
    //-----------------------------------------------------------------------------------------------------------------------------
    void DrawFillRect(const RectInt& rect, const Color& color)
    {
        Renderer::DrawFillRect(rect, color);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Draw a filled rect of a certain color.
    //-----------------------------------------------------------------------------------------------------------------------------
    void DrawFillRect(const RectF& rect, const Color& color)
    {
        Renderer::DrawFillRect(rect, color);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Draw an outlined rect of a certain color.
    //-----------------------------------------------------------------------------------------------------------------------------
    void DrawRect(const RectInt& rect, const Color& color)
    {
        Renderer::DrawRect(rect, color);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Draw an outlined rect of a certain color.
    //-----------------------------------------------------------------------------------------------------------------------------
    void DrawRect(const RectF& rect, const Color& color)
    {
        Renderer::DrawRect(rect, color);
    }

    void DrawCircle(const Vec2& pos, const float radius, const Color& color)
    {
        Renderer::DrawCircle(pos, radius, color);
    }

    void DrawTexture(const TextureRenderData& context)
    {
        Renderer::DrawTexture(context);
    }

}

