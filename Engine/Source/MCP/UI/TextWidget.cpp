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
    TextWidget::TextWidget(const WidgetConstructionData& data, const char* pText, const TextFormatData& textData)
        : Widget(data)
        , IRenderable(RenderLayer::kDebugOverlay, 0)
        , m_format(textData)
        , m_text(pText)
    {
        //
    }

    TextWidget::~TextWidget()
    {
        if (IsActive())
            m_pUILayer->RemoveRenderable(this);
    }

    bool TextWidget::Init()
    {
        if (IsActive())
            m_pUILayer->AddRenderable(this);

        // Load the font.
        if (!m_font.Load({{m_format.fontFilepath, nullptr, false } , m_format.fontSize }))
        {
            MCP_ERROR("TextWidget", "Failed to load font!");
            return false;
        }

        MCP_CHECK(m_font.IsValid());

        // Generate the Texture
        SetGlyphData();

        return true;
    }

    void TextWidget::Render() const
    {
        MCP_CHECK(m_font.IsValid());

        // Get the rect we are going to be in.
        RectF rect = GetRect();

        const auto center = GetRectCenter();
        rect.x = center.x;
        rect.y = center.y;

#ifdef _DEBUG
        // Debug Render the text Boundaries:
        //DrawRect( GetRectTopLeft(), Color::Black());
#endif
        
        const float startPosY = GetTextStartYPos(rect);
        int glyphIndex = 0;

        for(size_t i = 0; i < m_lines.size(); ++i)
        {
            auto& line = m_lines[i];

            // Get the start of the line according to the text alignment.
            const float startPosX = GetLineStartXPos(rect, i);

            // Debug:
            //DrawRect(RectF{startPosX, startPosY, static_cast<float>(line.lineWidth), static_cast<float>(line.lineHeight)}, Color::Black());

            for (size_t ii = 0; ii < line.glyphCount; ++ii)
            {
                const auto& glyphData = m_glyphs[glyphIndex];
                const RectF glyphDestRect
                {
                    static_cast<float>(glyphData.localPos.x) + startPosX,
                    static_cast<float>(glyphData.localPos.y) + startPosY,
                    static_cast<float>(glyphData.pTextureData->width),
                    static_cast<float>(glyphData.pTextureData->height)
                };

                TextureRenderData data;
                data.tint = {0,0,0};
                data.pTexture = glyphData.pTextureData->pTexture;
                data.destinationRect = glyphDestRect;
                data.crop = {0, 0, glyphData.pTextureData->width, glyphData.pTextureData->height};

                DrawTexture(data);

                ++glyphIndex;
            }
        }
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

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : When a parent Widget's ZOffset changes, our Renderable ZOrder needs to be updated.
    //-----------------------------------------------------------------------------------------------------------------------------
    void TextWidget::OnZChanged()
    {
        SetZOrder(GetZOffset());
    }

    void TextWidget::SetFont(const Font& font)
    {
        m_font = font;
        //RegenerateTextTexture();
        SetGlyphData();

        if (m_pParent)
            m_pParent->OnChildSizeChanged();
    }

    void TextWidget::SetText(const char* pText)
    {
        m_text = pText;
        //RegenerateTextTexture();
        SetGlyphData();

        if (m_pParent)
            m_pParent->OnChildSizeChanged();
    }

    void TextWidget::SetText(const std::string& text)
    {
        m_text = text;
        //RegenerateTextTexture();
        SetGlyphData();

        if (m_pParent)
            m_pParent->OnChildSizeChanged();
    }

    bool TextWidget::Append(const char character)
    {
        const auto glyph = static_cast<uint32_t>(character);
        auto* pTexture = m_font.GetGlyphTexture(glyph);
        if (!pTexture)
        {
            return false;
        }

        // Append the character to our string.
        m_text += character;

        // If this is the first glyph, then just add it.
        if (m_glyphs.empty())
        {
            m_lines.emplace_back(LineData{1, pTexture->width, m_font.GetFontHeight()});
            m_glyphs.emplace_back(pTexture, Vec2Int{} , glyph);

            m_textDimensions = {m_lines.back().lineWidth, m_lines.back().lineHeight };
        }

        // Otherwise, we need handle the potential new line case:
        else
        {
            // Get the width of the last line in the string.
            const auto& lastGlyph = m_glyphs.back();
            const auto distanceToPlaceGlyph = m_font.GetNextCharDistance(m_glyphs.back().glyph, glyph);
            const float rectWidth = GetScale().x * m_width;

            Vec2Int glyphPos = {lastGlyph.localPos.x + distanceToPlaceGlyph, 0 };

            // If adding this character would start a new line, we need move the word down.
            if (!m_sizedToContent && static_cast<float>(glyphPos.x) > rectWidth)
            {
                // Find the last space
                size_t lastSpaceIndex = m_glyphs.size() - 1;

                while (m_glyphs[lastSpaceIndex].glyph != ' ' && lastSpaceIndex > 0)
                {
                    --lastSpaceIndex;
                }

                // If the last word is the whole size, then just add it to the end, and don't create a new line.
                if (lastSpaceIndex == 0)
                {
                    glyphPos.x = lastGlyph.localPos.x + distanceToPlaceGlyph;
                    m_glyphs.emplace_back(pTexture, glyphPos, glyph);
                    m_lines.back().glyphCount += 1;
                    return true;
                }

                // Otherwise, we are creating a new line.
                m_lines.emplace_back(LineData{0, 0, m_font.GetNewlineDistance()});
                auto& lastLine = m_lines.back();

                // If the character is a space, then we just return without adding it.
                if (glyph == ' ')
                {
                    return true;
                }

                // Set the number of glyphs on the new line.
                lastLine.glyphCount = m_glyphs.size() - lastSpaceIndex;

                // Update our position to the new line.
                glyphPos.x = 0;
                glyphPos.y = lastLine.lineHeight;

                // Now we need to remove it the last space and move all of the characters down to the new line.
                for (size_t ii = lastSpaceIndex; ii < m_glyphs.size() - 1; ++ii)
                {
                    std::swap(m_glyphs[ii], m_glyphs[ii + 1]);

                    // Update the position of the swapped character to start on the new line.
                    m_glyphs[ii].localPos = glyphPos;

                    // Move the xPos over by the distance of the 'next' character, which is located after the
                    // space following the swap. 
                    if (ii < m_glyphs.size() - 2)
                    {
                        glyphPos.x += m_font.GetNextCharDistance(m_glyphs[ii].glyph, m_glyphs[ii + 2].glyph);
                    }
                }

                // Remove the space.
                m_glyphs.pop_back();
            }

            // Add the new character at the end:
            m_glyphs.emplace_back(pTexture, glyphPos, glyph);

            // Update our line with new width.
            m_lines.back().lineWidth = m_glyphs.back().localPos.x + m_glyphs.back().pTextureData->width;
            m_lines.back().glyphCount += 1;
        }

        if (m_pParent)
            m_pParent->OnChildSizeChanged();
        
        //MCP_LOG("TextWidget", "Line width: ", m_lines.back().lineWidth);

        return true;
    }

    void TextWidget::PopBack()
    {
        if (m_text.empty())
            return;

        // Remove the last character in the text
        m_text.pop_back();

        // Remove the last glyph
        m_glyphs.pop_back();

        // Update the last line.
        // If there are no glyphs, then we clear our lines.
        if (m_glyphs.empty())
        {
            m_lines.clear();
        }

        else
        {
            auto& line = m_lines.back();
            line.glyphCount -= 1;

            // If we have no more glyphs on this line, then return.
            if (line.glyphCount == 0)
            {
                m_lines.pop_back();
                return;
            }

            line.lineWidth = m_glyphs.back().localPos.x + m_glyphs.back().pTextureData->width;
        }

        if (m_pParent)
            m_pParent->OnChildSizeChanged();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the local offset position where the cursor should be for the current text.
    //-----------------------------------------------------------------------------------------------------------------------------
    Vec2 TextWidget::GetCursorPosAtEnd() const
    {
        // If we don't have any glyphs, then we just return our position.
        // NOTE: This is for centered.
        if (m_glyphs.empty())
            return Vec2(GetLocalLineStartXPos(0), GetLocalLineStartYPos(0));
        
        auto& lastGlyph = m_glyphs.back();

        // Get the start position of the text box.
        Vec2 position;
        position.x = GetLocalLineStartXPos(m_lines.size() - 1);

        // Move the cursor past the end of the last glyph.
        position.x += static_cast<float>(lastGlyph.localPos.x + m_font.GetCursorDistanceAfterGlyph(lastGlyph.glyph));
        position.y += static_cast<float>(lastGlyph.localPos.y);
        return position;
    }

    float TextWidget::GetRectWidth() const
    {
        if (m_sizedToContent)
        {
            return GetScale().x * static_cast<float>(m_textDimensions.x);
        }

        return GetScale().x * m_width;
    }

    float TextWidget::GetRectHeight() const
    {
        if (m_sizedToContent)
        {
            return GetScale().y * static_cast<float>(m_textDimensions.y);
        }

        return GetScale().y * m_height;
    }

    TextWidget* TextWidget::AddFromData(const XMLElement element)
    {
        // Get the basic Widget Data
        const auto data = GetWidgetConstructionData(element);
        
        // Get the Text
        const XMLElement child = element.GetChildElement("Text");
        const char* pText = child.GetText();

        // Format Data
        const auto formatData = GetTextFormatData(element);

        return BLEACH_NEW(TextWidget(data, pText, formatData));
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns TextFormatData from a XML file. The 'element' should be the root element of the Widget.
    //-----------------------------------------------------------------------------------------------------------------------------
    TextFormatData TextWidget::GetTextFormatData(const XMLElement element)
    {
        TextFormatData format;

        // FormatData
        const auto formatElement = element.GetChildElement("TextFormat");
        MCP_CHECK(formatElement.IsValid());

        // Font
        auto child = formatElement.GetChildElement("Font");
        MCP_CHECK(child.IsValid());
        format.fontFilepath = child.GetAttributeValue<const char*>("path");
        format.fontSize = child.GetAttributeValue<int>("size");

        // Colors
        child = child.GetSiblingElement("ForegroundColor");
        MCP_CHECK(child.IsValid());
        format.foreground.r = child.GetAttributeValue<uint8_t>("r");
        format.foreground.g = child.GetAttributeValue<uint8_t>("g");
        format.foreground.b = child.GetAttributeValue<uint8_t>("b");
        format.foreground.alpha = child.GetAttributeValue<uint8_t>("alpha");

        child = child.GetSiblingElement("BackgroundColor");
        MCP_CHECK(child.IsValid());
        format.background.r = child.GetAttributeValue<uint8_t>("r");
        format.background.g = child.GetAttributeValue<uint8_t>("g");
        format.background.b = child.GetAttributeValue<uint8_t>("b");
        format.background.alpha = child.GetAttributeValue<uint8_t>("alpha");

        child = child.GetSiblingElement("TextAlignment");
        if (child.IsValid())
        {
            format.alignment.x = std::clamp(child.GetAttributeValue<float>("x", 0.5f), 0.f, 1.f);
            format.alignment.y = std::clamp( child.GetAttributeValue<float>("y", 0.5f), 0.f, 1.f);
        }

        return format;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the start of a line of text based on the alignment - in screen space.
    ///		@param rect : The TextWidget's Rect.
    ///		@param lineIndex : Index of the line we are looking at.
    ///		@returns : X position of where the line should start being rendered.
    //-----------------------------------------------------------------------------------------------------------------------------
    float TextWidget::GetLineStartXPos(const RectF rect, const size_t lineIndex) const
    {
        MCP_CHECK(lineIndex < m_lines.size());
        const auto lineWidth = static_cast<float>(m_lines[lineIndex].lineWidth);
        const auto rectWidthScalar = m_format.alignment.x - 0.5f;

        const auto xPos = rect.x + (rectWidthScalar * rect.width) - (lineWidth * m_format.alignment.x);
        return xPos;
    }

    float TextWidget::GetLocalLineStartXPos(const size_t lineIndex) const
    {
        if (m_lines.empty())
        {
            const auto rectWidthScalar = m_format.alignment.x - 0.5f;
            return (rectWidthScalar * GetRectWidth());
        }

        MCP_CHECK(lineIndex < m_lines.size());
        const auto lineWidth = static_cast<float>(m_lines[lineIndex].lineWidth);
        const auto rectWidthScalar = m_format.alignment.x - 0.5f;
        const auto xPos = (rectWidthScalar * GetRectWidth()) - (lineWidth * m_format.alignment.x);

        return xPos;
    }

    float TextWidget::GetLocalLineStartYPos(const size_t lineIndex) const
    {
        if (m_lines.empty())
        {
            //const auto textHeight = static_cast<float>(lineIndex * m_font.GetFontHeight());
            const auto yPos = ((m_format.alignment.y - 0.5f ) * GetRectHeight());
            return yPos;
        }

        MCP_CHECK(lineIndex < m_lines.size());
        const auto textHeight = static_cast<float>(lineIndex * m_font.GetFontHeight());
        const auto yPos = ((m_format.alignment.y - 0.5f ) * GetRectHeight()) - (m_format.alignment.y * textHeight);

        return yPos;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the screen space position of the top of the 'text rect' - the rect that encompasses all of the lines of text.
    ///		@param rect : The TextWidget's Rect.
    ///		@returns : Y position where the text should start being rendered.
    //-----------------------------------------------------------------------------------------------------------------------------
    float TextWidget::GetTextStartYPos(const RectF rect) const
    {
        const auto textHeight = static_cast<float>(m_lines.size() * m_font.GetFontHeight());
        const auto yPos = rect.y + ((m_format.alignment.y - 0.5f ) * rect.height) - (m_format.alignment.y * textHeight);

        return yPos;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Recreates our glyph data for our current string.
    //-----------------------------------------------------------------------------------------------------------------------------
    void TextWidget::SetGlyphData()
    {
        m_glyphs.clear();
        m_lines.clear();
        m_textDimensions = {};
        if (m_text.empty())
            return;

        // Create our starting line.
        m_lines.emplace_back();
        m_lines.back().lineHeight = m_font.GetFontHeight();
        const float rectWidth = GetScale().x * m_width;
        Vec2Int glyphPos {};
        size_t lineStart = 0;
        size_t wordStart = 0;
        const int newLineDistance = m_font.GetNewlineDistance();

        // Space data:
        const auto* spaceGlyph = m_font.GetGlyphTexture(' ');
        MCP_CHECK(spaceGlyph);

        for (size_t i = 0; i < m_text.size(); ++i)
        {
            // While we have a word:
            while(m_text[i] != ' ' && i < m_text.size())
            {
                const auto glyph = static_cast<uint32_t>(m_text[i]);
                const auto* pGlyphTexture = m_font.GetGlyphTexture(glyph);
                if (!pGlyphTexture)
                {
                    ++i;
                    continue;
                }

                const auto distanceToPlaceGlyph = i == 0 ? 0 : m_font.GetNextCharDistance(m_glyphs.back().glyph, glyph);

                // If this glyph would put us on a new line,
                if (!m_sizedToContent && static_cast<float>(glyphPos.x + distanceToPlaceGlyph) > rectWidth)
                {
                    // If we have a space on the current line to go back to,
                    if (wordStart != lineStart)
                    {
                        m_lines.emplace_back();
                        auto& newLine = m_lines.back();

                        // Set our lineHeight
                        newLine.lineHeight = m_font.GetFontHeight();

                        // Set the new line start.
                        lineStart = wordStart - 1;
                        
                        // Set the position of the beginning of our new line.
                        glyphPos.y += newLineDistance;
                        glyphPos.x = 0;

                        // Move the space to the end and update the glyphs in the current words positions to
                        // be at the new line.
                        for (size_t ii = wordStart - 1; ii < m_glyphs.size() - 1; ++ii)
                        {
                            std::swap(m_glyphs[ii], m_glyphs[ii + 1]);

                            // Update the position of the swapped character to start on the new line.
                            m_glyphs[ii].localPos = glyphPos;

                            // Move the xPos over by the distance of the 'next' character, which is located after the
                            // space following the swap. 
                            if (ii < m_glyphs.size() - 2)
                                glyphPos.x += m_font.GetNextCharDistance(m_glyphs[ii].glyph, m_glyphs[ii + 2].glyph);
                        }

                        wordStart -= 1;

                        // Remove the space.
                        m_glyphs.pop_back();

                        const size_t glyphsRemovedFromLine = m_glyphs.size() - wordStart + 1;
                        m_lines[m_lines.size() - 2].glyphCount -= glyphsRemovedFromLine;

                        // Update the new line's glyph count.
                        newLine.glyphCount = glyphsRemovedFromLine - 1;
                    }
                }

                // Update our glyph position:
                glyphPos.x += distanceToPlaceGlyph;
                
                // Add the new glyph to the array.
                m_glyphs.emplace_back(pGlyphTexture, glyphPos, glyph);

                // Update our line Width
                m_lines.back().lineWidth = glyphPos.x + pGlyphTexture->width;
                m_lines.back().glyphCount += 1;

                // Increment our index in the string.
                ++i;
            }

            // If completing the word, set our textDimensions and break
            if (i >= m_text.size())
            {
                for (const auto& line : m_lines)
                {
                    if (line.lineWidth > m_textDimensions.x)
                        m_textDimensions.x = line.lineWidth;

                    m_textDimensions.y += line.lineHeight;
                }

                break;
            }

            // Add the space:
            const auto distanceToPlaceGlyph = i == 0 ? 0 : m_font.GetNextCharDistance(m_glyphs.back().glyph, ' ');

            // If adding the space will start a new line, update the positions but don't add it.
            if (static_cast<float>(glyphPos.x + distanceToPlaceGlyph) > rectWidth)
            {
                // Start a new line
                m_lines.emplace_back();
                m_lines.back().lineHeight = m_font.GetFontHeight();

                // I set the pos as the negative of the last width to negate the width that we are going to add later.
                glyphPos.x = -m_glyphs.back().pTextureData->width;
                glyphPos.y += newLineDistance;

                // The next line and word is on the 'next character'
                lineStart = m_glyphs.size(); 
                wordStart = m_glyphs.size();

                // Don't add the space and continue.
                continue;
            }

            // Update our glyph position:
            glyphPos.x += distanceToPlaceGlyph;
            
            // Add the space glyph to the array.
            m_glyphs.emplace_back(spaceGlyph, glyphPos, ' ');
            
            // Update our line Width
            m_lines.back().lineWidth = glyphPos.x + spaceGlyph->width;
            m_lines.back().glyphCount += 1;

            wordStart = m_glyphs.size();
        }
    }

    static int GetTextWidget(lua_State* pState)
    {
        // Get the Widget
        const auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);

        // Get the tag
        auto* pTag = lua_tostring(pState, -1);

        // Pop the parameters
        lua_pop(pState, 2);

        // Try to get the TextWidget.
        auto* pResult = pWidget->GetLayer()->GetWidgetByTag<TextWidget>(pTag);

        if (pResult)
            lua_pushlightuserdata(pState, pResult);

        else
            lua_pushnil(pState);

        return 1;
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

    static int GetWidgetText(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<TextWidget*>(lua_touserdata(pState, -1));
        MCP_CHECK(pWidget);

        // Pop the parameter.
        lua_pop(pState, 1);

        // Get the Widget's text.
        const auto& string = pWidget->GetText();
        lua_pushstring(pState, string.c_str());

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
            , {"GetTextWidget", &GetTextWidget}
            ,{nullptr, nullptr}
        };

        static constexpr luaL_Reg kTextFuncs[]
        {
             {"SetText", &SetWidgetText}
             ,{"GetText", &GetWidgetText}
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