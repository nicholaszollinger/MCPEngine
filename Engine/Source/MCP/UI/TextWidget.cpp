// TextWidget.cpp

#include "TextWidget.h"

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
        if (m_isActive)
            m_pUILayer->RemoveRenderable(this);

        // Free the text texture.
        FreeTexture();
    }

    bool TextWidget::Init()
    {
        m_pUILayer->AddRenderable(this);

        // Load the font.
        m_font.Load(m_textData.pFontFilepath, m_textData.fontSize);
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
        auto rect = GetScreenRect();

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

    void TextWidget::SetActive(const bool isActive)
    {
        if (isActive != m_isActive)
        {
            if (isActive)
            {
                m_pUILayer->AddRenderable(this);
            }

            else
            {
                m_pUILayer->RemoveRenderable(this);
            }

            m_isActive = isActive;
        }
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

    TextWidget* TextWidget::AddFromData(const XMLElement element)
    {
        // Get the basic Widget Data
        WidgetConstructionData data;
        RectF rect;
        rect.x = element.GetAttribute<float>("x");
        rect.y = element.GetAttribute<float>("y");
        rect.width = element.GetAttribute<float>("width");
        rect.height = element.GetAttribute<float>("height");

        data.rect = rect;
        data.anchor.x = element.GetAttribute<float>("anchorX", 0.5f);
        data.anchor.y = element.GetAttribute<float>("anchorY", 0.5f);
        data.isInteractable = false;
        data.zOffset = element.GetAttribute<int>("zOffset", 1);

        // Get the TextData
        TextData textData;
        const char* pText = element.GetAttribute<const char*>("text");

        // Font
        XMLElement child = element.GetChildElement("Font");
        MCP_CHECK(child.IsValid());
        textData.pFontFilepath = child.GetAttribute<const char*>("path");
        textData.fontSize = child.GetAttribute<int>("size");

        // Colors
        child = child.GetSiblingElement("ForegroundColor");
        MCP_CHECK(child.IsValid());
        textData.foreground.r = child.GetAttribute<uint8_t>("r");
        textData.foreground.g = child.GetAttribute<uint8_t>("g");
        textData.foreground.b = child.GetAttribute<uint8_t>("b");
        textData.foreground.alpha = child.GetAttribute<uint8_t>("alpha");

        child = child.GetSiblingElement("BackgroundColor");
        MCP_CHECK(child.IsValid());
        textData.background.r = child.GetAttribute<uint8_t>("r");
        textData.background.g = child.GetAttribute<uint8_t>("g");
        textData.background.b = child.GetAttribute<uint8_t>("b");
        textData.background.alpha = child.GetAttribute<uint8_t>("alpha");

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

    void TextWidget::OnParentActiveChanged(const bool parentActiveState)
    {
        // If we are being set active, add the renderable
        if (parentActiveState)
            m_pUILayer->AddRenderable(this);
        
        // And vice versa.
        else
            m_pUILayer->RemoveRenderable(this);
    }

}