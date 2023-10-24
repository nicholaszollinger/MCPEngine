// Font.cpp

#include "Font.h"

#include "MCP/Core/Resource/ResourceManager.h"

#if MCP_RENDERER_API == MCP_RENDERER_API_SDL
#include "Platform/SDL2/SDLText.h"

namespace mcp
{
    using FontAssetType = FontData;
}
#else
#error "We don't have a resource implementation for Texture loading for current API!'"
#endif

namespace mcp
{
    const TextureData* Font::GetGlyphTexture(const uint32_t glyph) const
    {
        if (glyph >= FontData::kGlyphCount)
        {
            MCP_WARN("Font", "Trying to get Glyph from outside of available range. Value must be between [0 (48), '~' (126)]. Glyph requested: ", glyph);
            return nullptr;
        }

        const auto* pFontData = static_cast<FontData*>(m_pResource);
        //MCP_CHECK(glyphIndex < pFontData->m_glyphTextures.size());

        return pFontData->m_glyphTextures[glyph];
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the distance vertically from the top of the current line of glyphs to the top of the next line of glyphs.
    //-----------------------------------------------------------------------------------------------------------------------------
    int Font::GetNewlineDistance() const
    {
        const auto* pFontData = static_cast<FontData*>(m_pResource);
        return GetNewLineDistance(static_cast<_TTF_Font*>(pFontData->pFontResource));
    }

    int Font::GetFontHeight() const
    {
        const auto* pFontData = static_cast<FontData*>(m_pResource);
        return SDLGetFontHeight(static_cast<_TTF_Font*>(pFontData->pFontResource));
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the distance horizontally to place the 'nextGlyph' next to the 'lastGlyph'.
    //-----------------------------------------------------------------------------------------------------------------------------
    int Font::GetNextCharDistance(const uint32_t lastGlyph, const uint32_t nextGlyph) const
    {
        const auto* pFontData = static_cast<FontData*>(m_pResource);
        return GetNextGlyphDistance(static_cast<_TTF_Font*>(pFontData->pFontResource), lastGlyph, nextGlyph);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the distance from the start of the glyph to where the cursor should be place after it.
    //-----------------------------------------------------------------------------------------------------------------------------
    int Font::GetCursorDistanceAfterGlyph(const uint32_t glyph) const
    {
        const auto* pFontData = static_cast<FontData*>(m_pResource);
        return GetCursorDistance(static_cast<_TTF_Font*>(pFontData->pFontResource), glyph);
    }


    void* Font::LoadResourceType()
    {
        return ResourceManager::Get()->LoadFromDisk<FontAssetType>(m_request);
    }

    void Font::Free()
    {
        ResourceManager::Get()->FreeResource<FontAssetType>(m_request);
    }

    void* Font::Get() const
    {
        if (!m_pResource)
            return nullptr;

        return static_cast<FontData*>(m_pResource)->pFontResource;
    }

}