#pragma once
// TextComponent.h

#include "Component.h"
#include "MCP/Core/Resource/Font.h"
#include "MCP/Graphics/Texture.h"
#include "MCP/Scene/IRenderable.h"
#include "Utility/Types/Color.h"

namespace mcp
{
    class TransformComponent;

    class TextComponent final : public Component, public IRenderable
    {
        MCP_DEFINE_COMPONENT_ID(TextComponent)

    public:
        struct TextComponentData
        {
            const char* pFontFilepath   = nullptr;
            int fontSize                = 0;
            int width                   = 0;
            int height                  = 0;
            Color foreground;
            Color background;

            // Any kind of styling?
        };

    private:
        TextComponentData m_data;   
        TransformComponent* m_pTransform;
        void* m_pTexture;                   // The texture.
        Vec2 m_size;                        // Size of the texture.
        std::string m_text;                 // Text that will be rendered.
        Font m_font;                        // The font resource that we are using.

    public:
        TextComponent(Object* pObject, const char* pText, const TextComponentData& data, const RenderLayer layer, const int zOrder);
        virtual ~TextComponent() override;

        virtual bool Init() override;
        virtual void Render() const override;

        void SetText(const char* pText);
        void SetFont(const Font& font);
        // TODO: More font sizing/styling
        static bool AddFromData(const XMLElement element, mcp::Object* pObject);

    private:
        void RegenerateTextTexture();
        void FreeTexture();
    };
}
