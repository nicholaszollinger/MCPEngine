// SDLText.cpp

#include "SDLText.h"

#pragma warning(push)
#pragma warning(disable : 26819)
#include <SDL_ttf.h>
#pragma warning(pop)

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

void FreeTextTexture(SDL_Texture* pTexture)
{
    SDL_DestroyTexture(pTexture);
}