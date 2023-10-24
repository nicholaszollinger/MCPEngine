#pragma once
// SDLText.h

#include "Utility/Types/Color.h"
#include "Utility/Types/Vector2.h"

struct _TTF_Font;
struct SDL_Texture;

struct TextGenerationData
{
    _TTF_Font* pFont        = nullptr;
    const char* pText       = nullptr;
    Color background;
    Color foreground;
    uint32_t wrapPixelLength = 0;
};

void SetFontSize(_TTF_Font* pFont, const int size);
int GetNewLineDistance(_TTF_Font* pFont);
int SDLGetFontHeight(_TTF_Font* pFont);
int GetNextGlyphDistance(_TTF_Font* pFont, const uint32_t lastGlyph, const uint32_t nextGlyph);
int GetCursorDistance(_TTF_Font* pFont, const uint32_t glyph);

SDL_Texture* GenerateTextTexture(Vec2Int& sizeOut, const TextGenerationData& data);
SDL_Texture* GenerateTextTextureWithBackground(Vec2Int& sizeOut, const TextGenerationData& data);
void FreeTextTexture(SDL_Texture* pTexture);