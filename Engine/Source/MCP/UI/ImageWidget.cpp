// ImageWidget.cpp

#include "ImageWidget.h"

#include "LuaSource.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Scene/UILayer.h"

namespace mcp
{
    ImageWidget::ImageWidget(const WidgetConstructionData& data, const char* pImageFilePath)
        : Widget(data)
        , IRenderable(RenderLayer::kDebugOverlay, 0)
        , m_crop{}
    {
        m_texture.Load(pImageFilePath);
        const auto imageSize = m_texture.GetBaseSize();
        m_crop = RectInt{0,0, imageSize.x, imageSize.y};
    }

    ImageWidget::~ImageWidget()
    {
        if (IsActive())
            m_pUILayer->RemoveRenderable(this);
    }

    bool ImageWidget::Init()
    {
        if (IsActive())
            m_pUILayer->AddRenderable(this);

        return true;
    }

    void ImageWidget::Render() const
    {
        auto rect = GetScreenRect();
        rect.x -= rect.width / 2.f;
        rect.y -= rect.height / 2.f;

        TextureRenderData renderData;
        renderData.pTexture = m_texture.Get();
        renderData.angle = m_renderAngle;
        renderData.crop = m_crop;
        renderData.tint = m_tint;
        renderData.destinationRect = rect;
        renderData.flip = m_flip;

        DrawTexture(renderData);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : When enabled, we are rendered.
    //-----------------------------------------------------------------------------------------------------------------------------
    void ImageWidget::OnEnable()
    {
        //MCP_LOG("UI", "Enabling Image Widget: ", *m_tag);
        m_pUILayer->AddRenderable(this);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : When disabled, we are not rendered.
    //-----------------------------------------------------------------------------------------------------------------------------
    void ImageWidget::OnDisable()
    {
        //MCP_LOG("UI", "Disabling Image Widget: ", *m_tag);
        m_pUILayer->RemoveRenderable(this);
    }

    ImageWidget* ImageWidget::AddFromData(const XMLElement element)
    {
        const auto data = GetWidgetConstructionData(element);
        const char* pFilepath = element.GetAttributeValue<const char*>("path");

        return BLEACH_NEW(ImageWidget(data, pFilepath));
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Attempts to find a child CanvasWidget.
    ///
    ///     \n LUA PARAMS: Widget* pWidget, const char* ChildWidgetName
    ///     \n RETURNS: userData pointer to the found Canvas Widget, or nullptr in the case of a fail.
    //-----------------------------------------------------------------------------------------------------------------------------
    static int GetChildImageWidgetByTag(lua_State* pState)
    {
        //MCP_LOG("Lua", "Calling GetChildCanvasWidgetByTag() from lua...");

        // Get the Widget and string parameters off the stack.
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);
        const auto* pTag = lua_tostring(pState, -1);
        MCP_CHECK(pTag);

        // Pop the two parameters
        lua_pop(pState, 2);

        // Try to find the result.
        auto* pResult = pWidget->FindChildByTag<ImageWidget>(pTag);

        // If valid, return a the data as a pointer.
        if (pResult)
            lua_pushlightuserdata(pState, pResult);
        // Otherwise, return nil.
        else
            lua_pushnil(pState);

        return 1;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Attempts to find the first child ImageWidget.
    ///
    ///     \n LUA PARAMS: Widget* pWidget
    ///     \n RETURNS: userData pointer to the found ImageWidget, or nullptr in the case of a fail.
    //-----------------------------------------------------------------------------------------------------------------------------
    static int GetFirstChildImageWidget(lua_State* pState)
    {
        //MCP_LOG("Lua", "Calling GetChildCanvasWidgetByTag() from lua...");

        // Get the Widget and string parameters off the stack.
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -1));
        MCP_CHECK(pWidget);

        // Pop the parameter
        lua_pop(pState, 1);

        // Try to find the result.
        auto* pResult = pWidget->FindFirstChildOfType<ImageWidget>();

        // If valid, return a the data as a pointer.
        if (pResult)
            lua_pushlightuserdata(pState, pResult);
        // Otherwise, return nil.
        else
            lua_pushnil(pState);

        return 1;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the tint of a Widget.
    ///
    ///     \n LUA PARAMS: Widget* pWidget, Color table.
    ///     \n RETURNS: VOID
    //-----------------------------------------------------------------------------------------------------------------------------
    static int SetImageWidgetTint(lua_State* pState)
    {
        //MCP_LOG("Lua", "Calling SetImageWidgetTint() from lua...");

        // Get the Widget and string parameters off the stack.
        auto* pWidget = static_cast<ImageWidget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);

        Color color{};
        // R
        lua_pushstring(pState, "r");
        bool success = lua_gettable(pState, -2);
        MCP_CHECK(success);
        color.r = static_cast<uint8_t>(lua_tointeger(pState, -1));
        lua_pop(pState, 1);

        // G
        lua_pushstring(pState, "g");
        success = lua_gettable(pState, -2);
        MCP_CHECK(success);
        color.g = static_cast<uint8_t>(lua_tointeger(pState, -1));
        lua_pop(pState, 1);

        // B
        lua_pushstring(pState, "b");
        success = lua_gettable(pState, -2);
        MCP_CHECK(success);
        color.b = static_cast<uint8_t>(lua_tointeger(pState, -1));
        lua_pop(pState, 1);

        // Alpha
        lua_pushstring(pState, "alpha");
        success = lua_gettable(pState, -2);
        MCP_CHECK(success);
        color.alpha = static_cast<uint8_t>(lua_tointeger(pState, -1));
        lua_pop(pState, 1);

        // Pop Widget* and the Color Table parameters
        lua_pop(pState, 2);

        // Try to find the result.
        pWidget->SetTint(color);

        return 0;
    }

    void ImageWidget::RegisterLuaFunctions(lua_State* pState)
    {
        lua_pushcfunction(pState, &GetChildImageWidgetByTag);
        // It would be cool to be able to set a global table 'ImageWidget' of functions.
        lua_setglobal(pState, "GetChildCanvasWidgetByTag");

        lua_pushcfunction(pState, &GetFirstChildImageWidget);
        lua_setglobal(pState, "GetChildImageWidget");

        lua_pushcfunction(pState , &SetImageWidgetTint);
        lua_setglobal(pState, "SetTint");
    }

}
