#pragma once
// TextWidget.h

#include "Widget.h"
#include "MCP/Core/Resource/Font.h"
#include "MCP/Scene/IRenderable.h"
#include "Utility/Types/Color.h"

namespace mcp
{
    struct TextFormatData
    {
        std::string fontFilepath   = {};
        Vec2 alignment               = {0.5f, 0.5f};
        int fontSize                = 0;
        int width                   = 0;
        int height                  = 0;
        Color foreground;
        Color background;

        // Any kind of styling?
    };

    class TextWidget final : public Widget, public IRenderable
    {
        MCP_DEFINE_WIDGET(TextWidget)

        struct GlyphData
        {
            GlyphData(const TextureData* pTexture, const Vec2Int pos, const uint32_t glyph)
                : pTextureData(pTexture)
                , localPos(pos)
                , glyph(glyph)
            {
                //
            }

            const TextureData* pTextureData = nullptr;  // The Texture of the glyph
            Vec2Int localPos;                              // Position of the Glyph relative to the Widget.
            uint32_t glyph;
        };

        struct LineData
        {
            size_t glyphCount = 0;
            int lineWidth = 0;
            int lineHeight = 0;
        };
        
        TextFormatData m_format;            // Data about the Text styling
        std::vector<GlyphData> m_glyphs;    // The textures that make up our string.
        std::vector<LineData> m_lines;      // Data regarding each line in our TextBox.
        std::string m_text;                 // Text that will be rendered.
        Font m_font;                        // The font resource that we are using.
        Vec2Int m_textDimensions;           // The total width and height of Text we are rendering.

    public:
        TextWidget(const WidgetConstructionData& data, const char* pText, const TextFormatData& format);
        virtual ~TextWidget() override;

        virtual bool Init() override;
        virtual void Render() const override;

        void SetText(const char* pText);
        void SetText(const std::string& text);
        void SetFont(const Font& font);
        bool Append(const char character);
        void PopBack();

        [[nodiscard]] size_t IsEmpty() const { return m_text.empty(); }
        [[nodiscard]] size_t GetTextLength() const { return m_text.size(); }
        [[nodiscard]] std::string& GetText() { return m_text; }
        [[nodiscard]] const std::string& GetText() const { return m_text; }
        [[nodiscard]] const Font& GetFont() const { return m_font; }
        [[nodiscard]] Vec2 GetCursorPosAtEnd() const;
        [[nodiscard]] virtual float GetRectWidth() const override;
        [[nodiscard]] virtual float GetRectHeight() const override;

        static TextWidget* AddFromData(const XMLElement element);
        static void RegisterLuaFunctions(lua_State* pState);
        static TextFormatData GetTextFormatData(const XMLElement element);

    private:
        [[nodiscard]] float GetLineStartXPos(const RectF rect, const size_t lineIndex) const;
        [[nodiscard]] float GetTextStartYPos(const RectF rect) const;
        [[nodiscard]] float GetLocalLineStartXPos(const size_t lineIndex) const;
        [[nodiscard]] float GetLocalLineStartYPos(const size_t lineIndex) const;
        void SetGlyphData();
        virtual void OnActive() override;
        virtual void OnInactive() override;
        virtual void OnParentSet() override;

    };
}
