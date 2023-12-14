// TextComponent.cpp

#include "TextComponent.h"

#include "MCP/Components/TransformComponent.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"

#if MCP_RENDERER_API == MCP_RENDERER_API_SDL
#include "Platform/SDL2/SDLText.h"
#endif

#define SHOW_TEXT_RECT 1

namespace mcp
{
    TextComponent::TextComponent(const char* pText, const TextComponentData& data, const RenderLayer layer, const int zOrder)
        : Component(true)
        , IRenderable(layer, zOrder)
        , m_data(data)
        , m_pTransform(nullptr)
        , m_pTexture(nullptr)
        , m_text(pText)
    {
        //
    }

    TextComponent::~TextComponent()
    {
        GetOwner()->GetWorld()->RemoveRenderable(this);
        FreeTexture();
    }

    bool TextComponent::Init()
    {
        GetOwner()->GetWorld()->AddRenderable(this);
        m_pTransform = GetOwner()->GetComponent<TransformComponent>();
        MCP_CHECK(m_pTransform);

        // Load the font.
        if (!m_font.Load({ DiskResourceRequest( m_data.pFontFilepath), m_data.fontSize}))
        {
            MCP_ERROR("TextWidget", "Failed to load font!");
            return false;
        }

        MCP_CHECK(m_font.IsValid());

        // Generate the texture.
        RegenerateTextTexture();

        return true;
    }

    void TextComponent::Render() const
    {
        MCP_CHECK(m_font.IsValid());
        MCP_CHECK(m_pTexture);

        const Vec2 location = m_pTransform->GetPosition();
        const float renderXPos = location.x - m_size.x / 2.f;
        const float renderYPos = location.y - m_size.y / 2.f;

        const RectF destination{renderXPos, renderYPos, m_size.x, m_size.y};

        TextureRenderData data;
        data.pTexture = m_pTexture;
        data.destinationRect = destination;

        // We want to always render the entire texture. The crop is always going to be the size of the texture.
        data.crop = RectInt(0,0, static_cast<int>(m_size.x),static_cast<int>(m_size.y));
        DrawTexture(data);

#if SHOW_TEXT_RECT
        DrawRect(destination, Color{255,100,0});
#endif
    }

    void TextComponent::SetFont(const Font& font)
    {
        m_font = font;
        RegenerateTextTexture();
    }

    void TextComponent::SetText(const char* pText)
    {
        m_text = pText;
        RegenerateTextTexture();
    }

    void TextComponent::OnActive()
    {
        GetOwner()->GetWorld()->AddRenderable(this);
    }

    void TextComponent::OnInactive()
    {
        GetOwner()->GetWorld()->RemoveRenderable(this);
    }

    TextComponent* TextComponent::AddFromData(const XMLElement element)
    {
        TextComponentData data;
        const char* pText = element.GetAttributeValue<const char*>("text");
        data.width = element.GetAttributeValue<int>("width");
        data.height = element.GetAttributeValue<int>("height");

        XMLElement child = element.GetChildElement("Font");
        MCP_CHECK(child.IsValid());

        data.pFontFilepath = child.GetAttributeValue<const char*>("path");
        data.fontSize = child.GetAttributeValue<int>("size");

        child = child.GetSiblingElement("ForegroundColor");
        MCP_CHECK(child.IsValid());
        data.foreground.r = child.GetAttributeValue<uint8_t>("r");
        data.foreground.g = child.GetAttributeValue<uint8_t>("g");
        data.foreground.b = child.GetAttributeValue<uint8_t>("b");
        data.foreground.alpha = child.GetAttributeValue<uint8_t>("alpha");

        child = child.GetSiblingElement("BackgroundColor");
        MCP_CHECK(child.IsValid());
        data.background.r = child.GetAttributeValue<uint8_t>("r");
        data.background.g = child.GetAttributeValue<uint8_t>("g");
        data.background.b = child.GetAttributeValue<uint8_t>("b");
        data.background.alpha = child.GetAttributeValue<uint8_t>("alpha");

        child = child.GetSiblingElement("Renderable");
        MCP_CHECK(child.IsValid());
        const auto layer = static_cast<RenderLayer>(child.GetAttributeValue<int>("layer"));
        const int zOrder = child.GetAttributeValue<int>("zOrder");

        return BLEACH_NEW(TextComponent(pText, data, layer, zOrder));
    }

    void TextComponent::FreeTexture()
    {
        // Free the Texture.
        FreeTextTexture(static_cast<SDL_Texture*>(m_pTexture));
        m_pTexture = nullptr;
    }

    void TextComponent::RegenerateTextTexture()
    {
        if (m_pTexture)
            FreeTexture();

        TextGenerationData data;
        data.pFont = static_cast<_TTF_Font*>(m_font.Get());
        data.pText = m_text.c_str();
        data.foreground = m_data.foreground;
        data.background = m_data.background;
        data.wrapPixelLength = m_data.width;

        Vec2Int textureSize;
        m_pTexture = GenerateTextTextureWithBackground(textureSize, data);

        m_size.x = static_cast<float>(textureSize.x);
        m_size.y = static_cast<float>(textureSize.y);
    }
}


