// TextWidget.cpp

#include "TextWidget.h"

#include "LuaSource.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Scene/UILayer.h"

#if MCP_RENDERER_API == MCP_RENDERER_API_SDL
#include "Platform/SDL2/SDLText.h"
#endif

namespace mcp
{
    TextWidget::TextWidget(const WidgetConstructionData& data, const char* pText, const TextData& textData)
        : Widget(data)
        , IRenderable(RenderLayer::kDebugOverlay, 0)
        , m_crop({ })
        , m_textData(textData)
        , m_pTexture(nullptr)
        , m_text(pText)
    {
        //
    }

    TextWidget::~TextWidget()
    {
        if (IsActive())
            m_pUILayer->RemoveRenderable(this);

        // Free the text texture.
        FreeTexture();
    }

    bool TextWidget::Init()
    {
        if (IsActive())
            m_pUILayer->AddRenderable(this);

        // Load the font.
        if (!m_font.Load({{m_textData.pFontFilepath, nullptr, false } , m_textData.fontSize }))
        {
            MCP_ERROR("TextWidget", "Failed to load font!");
            return false;
        }

        MCP_CHECK(m_font.IsValid());

        // Generate the Texture
        RegenerateTextTexture();

        return true;
    }

    void TextWidget::Render() const
    {
        MCP_CHECK(m_font.IsValid());
        MCP_CHECK(m_pTexture);

        // Get the rect we are going to be in.
        auto rect = GetRect();

        // Set the rect position to the top left corner of the text rect.
        //  TODO: This adjustment could be the 'justify' adjustment for the text. The alignment. But I don't know how to adjust each line.
        rect.x -= static_cast<float>(m_crop.width) / 2.f;
        rect.y -= static_cast<float>(m_crop.height) / 2.f;

        // Adjust the size to match the text size.
        rect.width = static_cast<float>(m_crop.width);
        rect.height = static_cast<float>(m_crop.height);
        //DrawRect(rect, {255,0,0});

        TextureRenderData data;
        data.pTexture = m_pTexture;
        data.destinationRect = rect;
        data.crop = m_crop;

        DrawTexture(data);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : When this is enabled, we register as a renderable.
    //-----------------------------------------------------------------------------------------------------------------------------
    void TextWidget::OnActive()
    {
        m_isVisible = true;
        m_pUILayer->AddRenderable(this);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : When this is disabled, we unregister from rendering. 
    //-----------------------------------------------------------------------------------------------------------------------------
    void TextWidget::OnInactive()
    {
        m_isVisible = false;
        m_pUILayer->RemoveRenderable(this);
    }

    void TextWidget::OnParentSet()
    {
        // When we set the parent, we need to update our zOrder to match the widget zOffset.
        SetZOrder(GetZOffset());
    }


    void TextWidget::SetFont(const Font& font)
    {
        m_font = font;
        RegenerateTextTexture();
    }

    void TextWidget::SetText(const char* pText)
    {
        m_text = pText;
        RegenerateTextTexture();
    }

    float TextWidget::GetRectWidth() const
    {
        if (m_sizedToContent)
        {
            return GetScale().x * static_cast<float>(m_crop.width);
        }

        return GetScale().x * m_width;
    }

    float TextWidget::GetRectHeight() const
    {
        if (m_sizedToContent)
        {
            return GetScale().y * static_cast<float>(m_crop.height);
        }

        return GetScale().y * m_height;
    }

    TextWidget* TextWidget::AddFromData(const XMLElement element)
    {
        // Get the basic Widget Data
        const auto data = GetWidgetConstructionData(element);

        // Get the TextData
        TextData textData;
        const char* pText = element.GetAttributeValue<const char*>("text");

        // Font
        XMLElement child = element.GetChildElement("Font");
        MCP_CHECK(child.IsValid());
        textData.pFontFilepath = child.GetAttributeValue<const char*>("path");
        textData.fontSize = child.GetAttributeValue<int>("size");

        // Colors
        child = child.GetSiblingElement("ForegroundColor");
        MCP_CHECK(child.IsValid());
        textData.foreground.r = child.GetAttributeValue<uint8_t>("r");
        textData.foreground.g = child.GetAttributeValue<uint8_t>("g");
        textData.foreground.b = child.GetAttributeValue<uint8_t>("b");
        textData.foreground.alpha = child.GetAttributeValue<uint8_t>("alpha");

        child = child.GetSiblingElement("BackgroundColor");
        MCP_CHECK(child.IsValid());
        textData.background.r = child.GetAttributeValue<uint8_t>("r");
        textData.background.g = child.GetAttributeValue<uint8_t>("g");
        textData.background.b = child.GetAttributeValue<uint8_t>("b");
        textData.background.alpha = child.GetAttributeValue<uint8_t>("alpha");

        return BLEACH_NEW(TextWidget(data, pText, textData));
    }

    void TextWidget::FreeTexture()
    {
        // Free the Texture.
        FreeTextTexture(static_cast<SDL_Texture*>(m_pTexture));
        m_pTexture = nullptr;
    }

    void TextWidget::RegenerateTextTexture()
    {
        if (m_pTexture)
            FreeTexture();

        TextGenerationData data;
        data.pFont = static_cast<_TTF_Font*>(m_font.Get());
        data.pText = m_text.c_str();
        data.foreground = m_textData.foreground;
        data.background = m_textData.background;

        // This matches the wrap length to the width of the text.
        data.wrapPixelLength = m_textData.width;

        Vec2Int textureSize;
        m_pTexture = GenerateTextTexture(textureSize, data);

        // Update the crop.
        m_crop.width = textureSize.x;
        m_crop.height = textureSize.y;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Attempts to find a child TextWidget.
    ///
    ///     \n LUA PARAMS: Widget* pWidget
    ///     \n RETURNS: userData pointer to the found Text Widget, or nullptr in the case of a fail.
    //-----------------------------------------------------------------------------------------------------------------------------
    static int GetFirstChildTextWidget(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -1));
        MCP_CHECK(pWidget);

        lua_pop(pState, 1);

        // Try to find the result.
        auto* pResult = pWidget->FindFirstChildOfType<TextWidget>();

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
    ///		@brief : Sets the text of a TextWidget
    ///
    ///     \n LUA PARAMS: TextWidget* pWidget, const char* pText
    ///     \n RETURNS: VOID
    //-----------------------------------------------------------------------------------------------------------------------------
    static int SetWidgetText(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<TextWidget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);

        auto* pText = lua_tostring(pState, -1);

        // Pop the parameters.
        lua_pop(pState, 2);

        pWidget->SetText(pText);

        return 0;
    }

    void TextWidget::RegisterLuaFunctions(lua_State* pState)
    {
        static constexpr luaL_Reg kFuncs[]
        {
             {"GetFirstChildTextWidget", &GetFirstChildTextWidget}
            ,{nullptr, nullptr}
        };

        static constexpr luaL_Reg kTextFuncs[]
        {
             {"SetText", &SetWidgetText}
            ,{nullptr, nullptr}
        };

        // Set the Widget Functions
        lua_getglobal(pState, "Widget");
        MCP_CHECK(lua_type(pState, -1) == LUA_TTABLE);
        luaL_setfuncs(pState, kFuncs, 0);
        // Pop the table off the stack.
        lua_pop(pState, 1);

        // Set the Text Widget Functions
        lua_getglobal(pState, "TextWidget");
        MCP_CHECK(lua_type(pState, -1) == LUA_TTABLE);
        luaL_setfuncs(pState, kTextFuncs, 0);
        // Pop the table off the stack.
        lua_pop(pState, 1);
    }

}