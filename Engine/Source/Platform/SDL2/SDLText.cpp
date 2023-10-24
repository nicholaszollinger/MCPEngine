// SDLText.cpp

#include "SDLText.h"

#pragma warning(push)
#pragma warning(disable : 26819)
#include <SDL_ttf.h>
#pragma warning(pop)

#include "SDLText.h"

#include "SDLHelpers.h"
#include "MCP/Debug/Assert.h"

void SetFontSize(TTF_Font* pFont, const int size)
{
    TTF_SetFontSize(pFont, size);
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Generates the texture for text in a particular font.
///		@param sizeOut : The final size of the texture.
///		@param data : Data to instruct how the texture will be formed.
///		@returns : Pointer to the new texture, or nullptr if something went wrong.
//-----------------------------------------------------------------------------------------------------------------------------
SDL_Texture* GenerateTextTexture(Vec2Int& sizeOut, const TextGenerationData& data)
{
    MCP_CHECK(data.pFont);
    auto* pSurface = TTF_RenderText_Blended_Wrapped( data.pFont, data.pText, mcp::ColorToSdl(data.foreground), data.wrapPixelLength);
    MCP_CHECK(pSurface);

    return mcp::CreateTextureFromSurface(pSurface, sizeOut);
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Generates the texture for text, with a background.
///		@param sizeOut : The final size of the texture.
///		@param data : Data to instruct how the texture will be formed.
///		@returns : Pointer to the new texture, or nullptr if something went wrong.
//-----------------------------------------------------------------------------------------------------------------------------
SDL_Texture* GenerateTextTextureWithBackground(Vec2Int& sizeOut, const TextGenerationData& data)
{
    MCP_CHECK(data.pFont);
    auto* pSurface = TTF_RenderText_Shaded_Wrapped( data.pFont, data.pText, mcp::ColorToSdl(data.foreground), mcp::ColorToSdl(data.background), data.wrapPixelLength);
    MCP_CHECK(pSurface);

    return mcp::CreateTextureFromSurface(pSurface, sizeOut);
}

int GetNewLineDistance(_TTF_Font* pFont)
{
    return TTF_FontLineSkip(pFont) + TTF_FontDescent(pFont);
}

int SDLGetFontHeight(_TTF_Font* pFont)
{
    return TTF_FontHeight(pFont);
}

int GetNextGlyphDistance(_TTF_Font* pFont, const uint32_t lastGlyph, const uint32_t nextGlyph)
{
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    int advance;

    TTF_GlyphMetrics32(pFont, lastGlyph, &xMin, &xMax, &yMin, &yMax, &advance);
    const int kerningSize = TTF_GetFontKerningSizeGlyphs32(pFont, lastGlyph, nextGlyph);

    // This is the case for spaces.
    if (xMax + kerningSize == 0)
        return advance;

    return xMax + kerningSize;
}

int GetCursorDistance(_TTF_Font* pFont, const uint32_t glyph)
{
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    int advance;

    TTF_GlyphMetrics32(pFont, glyph, &xMin, &xMax, &yMin, &yMax, &advance);
    return advance;
}


void FreeTextTexture(SDL_Texture* pTexture)
{
    SDL_DestroyTexture(pTexture);
}