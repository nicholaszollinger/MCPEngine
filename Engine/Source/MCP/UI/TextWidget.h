#pragma once
// TextWidget.h

#include "Widget.h"
#include "MCP/Core/Resource/Font.h"
#include "MCP/Scene/IRenderable.h"
#include "Utility/Types/Color.h"

namespace mcp
{
    class TextWidget final : public Widget, public IRenderable
    {
        MCP_DEFINE_WIDGET(TextWidget)

    public:
        struct TextData
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
        RectInt m_crop;
        TextData m_textData;   // Data about the Text styling
        void* m_pTexture;           // The texture.
        std::string m_text;         // Text that will be rendered.
        Font m_font;                // The font resource that we are using.

    public:
        TextWidget(const WidgetConstructionData& data, const char* pText, const TextData&);
        virtual ~TextWidget() override;

        virtual bool Init() override;
        virtual void Render() const override;

        void SetText(const char* pText);
        void SetFont(const Font& font);

        static TextWidget* AddFromData(const XMLElement element);

    private:
        void FreeTexture();
        void RegenerateTextTexture();
    };
}
