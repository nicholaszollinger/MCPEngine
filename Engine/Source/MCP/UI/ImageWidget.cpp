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
        if (!pImageFilePath)
        {
            return;
        }

        if (!m_texture.Load(DiskResourceRequest(pImageFilePath)))
        {
            MCP_ERROR("ImageWidget", "Failed to load texture!");
            return;
        }

        const auto imageSize = m_texture.GetTextureSize();
        m_crop = RectInt{0,0, imageSize.x, imageSize.y};
    }

    bool ImageWidget::Init()
    {
        if (IsActive() && m_texture.IsValid())
            GetUILayer()->AddRenderable(this);

        return true;
    }

    void ImageWidget::Render() const
    {
        const auto visibleRect = GetVisibleRect();

        if (!visibleRect.HasValidDimensions())
            return;

        const auto topLeft = GetRectTopLeft();
        const auto finalCropX = m_crop.x + static_cast<int>((visibleRect.x - topLeft.x));
        const auto finalCropY = m_crop.y + static_cast<int>((visibleRect.y - topLeft.y));

        const float normalizedWidth = visibleRect.width / topLeft.width;
        const float normalizedHeight = visibleRect.height / topLeft.height;
        const int finalCropWidth = std::clamp(static_cast<int>(normalizedWidth * static_cast<float>(m_crop.width)), 0, m_crop.width);
        const int finalCropHeight = std::clamp(static_cast<int>(normalizedHeight * static_cast<float>(m_crop.height)), 0, m_crop.height);

        const RectInt finalCrop = 
        {
            finalCropX,
            finalCropY,
            finalCropWidth,
            finalCropHeight
        };

        TextureRenderData renderData;
        renderData.pTexture = m_texture.Get();
        renderData.angle = m_renderAngle;
        renderData.crop = finalCrop;
        renderData.tint = m_tint;
        renderData.anglePivot = m_anglePivot;
        renderData.destinationRect = visibleRect;
        renderData.flip = m_flip;

        DrawTexture(renderData);
    }

    float ImageWidget::GetRectWidth() const
    {
        if (m_sizedToContent)
        {
            return GetScale().x * m_texture.GetTextureSizeAsVec2().x;
        }

        return GetScale().x * m_width;
    }

    float ImageWidget::GetRectHeight() const
    {
        if (m_sizedToContent)
        {
            return GetScale().y * m_texture.GetTextureSizeAsVec2().y;
        }

        return GetScale().y * m_height;
    }

    void ImageWidget::SetTexture(const Texture* pTexture)
    {
        if (!pTexture || !pTexture->IsValid())
        {
            // If we were already invalid, return.
            if (!m_texture.IsValid())
                return;

            m_texture = kInvalidTexture;

            if (IsActive() && m_isVisible)
            {
                m_isVisible = false;
                GetUILayer()->RemoveRenderable(this);
            }
        }

        else
        {
            m_texture = *pTexture;

            if (m_sizedToContent)
            {
                const auto imageSize = m_texture.GetTextureSize();
                m_crop = RectInt{0,0, imageSize.x, imageSize.y};
            }

            if (IsActive() && !m_isVisible)
            {
                m_isVisible = true;
                GetUILayer()->AddRenderable(this);
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : When enabled, we are rendered.
    //-----------------------------------------------------------------------------------------------------------------------------
    void ImageWidget::OnActive()
    {
        if (m_texture.IsValid())
        {
            m_isVisible = true;
            GetUILayer()->AddRenderable(this);
        }

        Widget::OnActive();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : When disabled, we are not rendered.
    //-----------------------------------------------------------------------------------------------------------------------------
    void ImageWidget::OnInactive()
    {
        if (m_texture.IsValid())
        {
            m_isVisible = false;
            GetUILayer()->RemoveRenderable(this);
        }

        Widget::OnInactive();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : When a parent Widget's ZOffset changes, our Renderable ZOrder needs to be updated.
    //-----------------------------------------------------------------------------------------------------------------------------
    void ImageWidget::OnZChanged()
    {
        SetZOrder(GetZOffset());
    }

    ImageWidget* ImageWidget::AddFromData(const XMLElement element)
    {
        const auto data = GetWidgetConstructionData(element);
        const char* pFilepath = element.GetAttributeValue<const char*>("path", nullptr);

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
        //MCP_LOG("Lua", "Calling GetChildCanvasWidget() from lua...");

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

    static int FindImageWidgetByTag(lua_State* pState)
    {
        // Get the Widget and string parameters off the stack.
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);
        const auto* pTag = lua_tostring(pState, -1);
        MCP_CHECK(pTag);

        // Pop the two parameters
        lua_pop(pState, 2);

        auto* pResult = pWidget->GetUILayer()->GetWidgetByTag(pTag);

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
        if (!pWidget)
        {
            lua_pop(pState, 2);
            return 0;
        }

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

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the a table containing the x, y, width, & height values of the crop.
    ///
    ///     \n LUA PARAMS: ImageWidget* pWidget
    ///     \n RETURNS: VOID
    //-----------------------------------------------------------------------------------------------------------------------------
    static int GetImageCrop(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<ImageWidget*>(lua_touserdata(pState, -1));
        MCP_CHECK(pWidget);

        // Get the crop of the Widget.
        RectInt crop = pWidget->GetCrop();

        // Pop the Widget parameter
        lua_pop(pState, 1);

        // Create a table
        lua_newtable(pState);

        // Add the x
        lua_pushinteger(pState, crop.x);
        lua_setfield(pState, -2, "x");

        // Add the y
        lua_pushinteger(pState, crop.y);
        lua_setfield(pState, -2, "y");

        // Add width
        lua_pushinteger(pState, crop.width);
        lua_setfield(pState, -2, "width");

        // Add Height
        lua_pushinteger(pState, crop.height);
        lua_setfield(pState, -2, "height");

        return 1;
    }

    static int SetImageCrop(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<ImageWidget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);

        RectInt crop{};
        // X
        lua_pushstring(pState, "x");
        bool success = lua_gettable(pState, -2);
        MCP_CHECK(success);
        crop.x = static_cast<int>(lua_tointeger(pState, -1));
        lua_pop(pState, 1);

        // Y
        lua_pushstring(pState, "y");
        success = lua_gettable(pState, -2);
        MCP_CHECK(success);
        crop.y = static_cast<int>(lua_tointeger(pState, -1));
        lua_pop(pState, 1);

        // Width
        lua_pushstring(pState, "width");
        success = lua_gettable(pState, -2);
        MCP_CHECK(success);
        if (lua_type(pState, -1) == LUA_TNUMBER)
        {
            crop.width = static_cast<int>(lua_tonumber(pState, -1));
        }

        else
        {
            crop.width = static_cast<int>(lua_tointeger(pState, -1));
        }

        lua_pop(pState, 1);

        // Height
        lua_pushstring(pState, "height");
        success = lua_gettable(pState, -2);
        MCP_CHECK(success);
        crop.height = static_cast<int>(lua_tointeger(pState, -1));
        lua_pop(pState, 1);

        // Pop the ImageWidget* and RectInt table params.
        lua_pop(pState, 2);

        // Set the crop of the Image
        pWidget->SetCrop(crop);

        return 0;
    }

    static int ScriptSetTexture(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<ImageWidget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);

        // Get the texture pointer
        auto* pTexture = static_cast<Texture*>(lua_touserdata(pState, -1));
        //MCP_CHECK(pTexture);

        // Pop the parameters
        lua_pop(pState, 2);

        // Set the Texture:
        pWidget->SetTexture(pTexture);

        return 0;
    }

    void ImageWidget::RegisterLuaFunctions(lua_State* pState)
    {
        static constexpr luaL_Reg kFuncs[]
        {
            {"SetTint", &SetImageWidgetTint}
             ,{"GetChildImageWidgetByTag", &GetChildImageWidgetByTag}
             ,{"GetFirstChildImageWidget", &GetFirstChildImageWidget}
            ,{"FindImageWidgetByTag", &FindImageWidgetByTag}
            ,{nullptr, nullptr}
        };

        static constexpr luaL_Reg kImageWidgetFuncs[]
        {
             {"SetTint", &SetImageWidgetTint}
             ,{"SetTexture", &ScriptSetTexture}
            ,{"GetCrop", &GetImageCrop}
            ,{"SetCrop", &SetImageCrop}
            ,{nullptr, nullptr}
        };

        // Set the Widget Functions:
        lua_getglobal(pState, "Widget");
        MCP_CHECK(lua_type(pState, -1) == LUA_TTABLE);
        luaL_setfuncs(pState, kFuncs, 0);

        // Pop the table off the stack.
        lua_pop(pState, 1);

        // Set the ImageWidget Functions:
        lua_getglobal(pState, "ImageWidget");
        MCP_CHECK(lua_type(pState, -1) == LUA_TTABLE);
        luaL_setfuncs(pState, kImageWidgetFuncs, 0);

        // Pop the table off the stack.
        lua_pop(pState, 1);
    }

}
