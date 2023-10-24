// SDLResources.cpp

#include "MCP/Core/Resource/ResourceManager.h"

#pragma warning(push)
#pragma warning(disable : 26819)
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#pragma warning(pop)

#include "MCP/Core/Application/Window/WindowBase.h"
#include "Platform/SDL2/SDLHelpers.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Graphics/Texture.h"
#include "MCP/Core/Resource/Font.h"

namespace mcp
{
#if MCP_RENDERER_API == MCP_RENDERER_API_SDL
    //--------------------------------------------------------------------------------------------------------
    //      SDL_TEXTURES
    //--------------------------------------------------------------------------------------------------------

    template <>
    TextureData* ResourceContainer<TextureData, DiskResourceRequest>::LoadFromDiskImpl(const DiskResourceRequest& request)
    {
        SDL_Surface* pSurface = IMG_Load(request.path.GetCStr());

        if (!pSurface)
        {
            MCP_ERROR("SDL", "Failed to Load SDL_Surface at filepath: ", request.path.GetCStr(), ". SDL_Error: ", SDL_GetError());
            return nullptr;
        }

        Vec2Int sizeOut = {};
        auto* pTexture = CreateTextureFromSurface(pSurface, sizeOut);

        return BLEACH_NEW(TextureData(pTexture, sizeOut.x, sizeOut.y));
    }

    template <>
    TextureData* ResourceContainer<TextureData, DiskResourceRequest>::LoadFromRawDataImpl(char* pRawData, const int dataSize, [[maybe_unused]] const DiskResourceRequest& request)
    {
        SDL_RWops* pSdlData = SDL_RWFromMem(pRawData, dataSize);
        // TODO: Check for nullptr.

        SDL_Surface* pSurface = IMG_Load_RW(pSdlData, 0);

        if (!pSurface)
        {
            MCP_ERROR("SDL", "Failed to Load SDL_Surface from raw data! SDL_Error: ", SDL_GetError());
            return nullptr;
        }

        Vec2Int sizeOut = {};
        auto* pTexture = CreateTextureFromSurface(pSurface, sizeOut);

        return BLEACH_NEW(TextureData(pTexture, sizeOut.x, sizeOut.y));
    }

    template<>
    void ResourceContainer<TextureData, DiskResourceRequest>::FreeResourceImpl(TextureData* pTextureData)
    {
        SDL_DestroyTexture(static_cast<SDL_Texture*>(pTextureData->pTexture));
        BLEACH_DELETE(pTextureData);
        pTextureData = nullptr;
    }
#endif

    //--------------------------------------------------------------------------------------------------------
    //      MIX_CHUNK
    //--------------------------------------------------------------------------------------------------------

    template <>
    Mix_Chunk* ResourceContainer<Mix_Chunk, DiskResourceRequest>::LoadFromDiskImpl(const DiskResourceRequest& request)
    {
        auto* pChunk = Mix_LoadWAV(request.path.GetCStr());
        if (!pChunk)
        {
            MCP_ERROR("SDL", "Failed to load Mix_Chunk at filepath: ", request.path.GetCStr());
            return nullptr;
        }

        return pChunk;
    }

    template <>
    Mix_Chunk* ResourceContainer<Mix_Chunk, DiskResourceRequest>::LoadFromRawDataImpl(char* pRawData, const int dataSize, [[maybe_unused]] const DiskResourceRequest& request)
    {
        SDL_RWops* pSdlData = SDL_RWFromMem(pRawData, dataSize);
        auto* pChunk = Mix_LoadWAV_RW(pSdlData, 0);
        if (!pChunk)
        {
            MCP_ERROR("SDL", "Failed to load Mix_Chunk from data! SDL_Error: ", Mix_GetError());
            return nullptr;
        }

        return pChunk;
    }

    template <>
    void ResourceContainer<Mix_Chunk, DiskResourceRequest>::FreeResourceImpl(Mix_Chunk* pChunk)
    {
        Mix_FreeChunk(pChunk);
    }

    //--------------------------------------------------------------------------------------------------------
    //      MIX_MUSIC
    //--------------------------------------------------------------------------------------------------------

    template <>
    Mix_Music* ResourceContainer<Mix_Music, DiskResourceRequest>::LoadFromDiskImpl(const DiskResourceRequest& request)
    {
        auto* pMusic = Mix_LoadMUS(request.path.GetCStr());
        if (!pMusic)
        {
            MCP_ERROR("SDL", "Failed to load Mix_Music at filepath: ", request.path.GetCStr());
            return nullptr;
        }

        return pMusic;
    }

    template <>
    Mix_Music* ResourceContainer<Mix_Music, DiskResourceRequest>::LoadFromRawDataImpl(char* pRawData, const int dataSize, [[maybe_unused]] const DiskResourceRequest& request)
    {
        SDL_RWops* pSdlData = SDL_RWFromMem(pRawData, dataSize);
        auto* pMusic = Mix_LoadMUS_RW(pSdlData, 0);
        if (!pMusic)
        {
            MCP_ERROR("SDL", "Failed to load Mix_Chunk from data! SDL_Error: ", Mix_GetError());
            return nullptr;
        }

        return pMusic;
    }

    template <>
    void ResourceContainer<Mix_Music, DiskResourceRequest>::FreeResourceImpl(Mix_Music* pFont)
    {
        Mix_FreeMusic(pFont);
    }

    //--------------------------------------------------------------------------------------------------------
    //      TTF_FONTS
    //--------------------------------------------------------------------------------------------------------

    static TextureData* CreateGlyphTexture(const uint32_t glyphCode, _TTF_Font* pFont)
    {
        static constexpr SDL_Color kColor = {0,0,0,255};

        // If the font does not support the code, then return nullptr
        if (TTF_GlyphIsProvided32(pFont, glyphCode) == 0)
            return nullptr;

        auto* pSurface = TTF_RenderGlyph32_Blended(pFont, glyphCode, kColor);

        Vec2Int imageSize;
        auto* pTexture = CreateTextureFromSurface(pSurface, imageSize);
        if (!pTexture)
        {
            MCP_ERROR("SDL", "Failed to create Glyph texture. Glyph code: ", glyphCode);
            return nullptr;
        }

        return BLEACH_NEW(TextureData(pTexture, imageSize.x, imageSize.y));
    }

    template <>
    FontData* ResourceContainer<FontData, FontResourceRequest>::LoadFromDiskImpl(const FontResourceRequest& request)
    {
        auto* pFont = TTF_OpenFont(request.path.GetCStr(), request.fontSize);

        if (!pFont)
        {
            MCP_ERROR("SDL", "Failed to load TTF_Font from data! TTF_Error: ", TTF_GetError());
            return nullptr;
        }

        // Create the resource:
        auto* pFontData = BLEACH_NEW(FontData);
        pFontData->pFontResource = pFont;
        pFontData->m_glyphTextures.reserve(FontData::kGlyphCount);

        // Create the Glyphs for the font.
        for(uint32_t i = 0; i < FontData::kGlyphCount; ++i)
        {
            auto* pGlyphTextureData = CreateGlyphTexture(i, pFont);
            if (!pGlyphTextureData)
            {
                pFontData->m_glyphTextures.emplace_back(nullptr);
                continue;
            }

            pFontData->m_glyphTextures.emplace_back(pGlyphTextureData);
        }

        return pFontData;
    }

    template <>
    FontData* ResourceContainer<FontData, FontResourceRequest>::LoadFromRawDataImpl(char* pRawData, const int dataSize, const FontResourceRequest& request)
    {
        SDL_RWops* pSdlData = SDL_RWFromMem(pRawData, dataSize);
        auto* pFont = TTF_OpenFontRW(pSdlData, 0, request.fontSize);
        if (!pFont)
        {
            MCP_ERROR("SDL", "Failed to load TTF_Font from raw data! TTF_Error: ", TTF_GetError());
            return nullptr;
        }

        // Create the resource:
        auto* pFontData = BLEACH_NEW(FontData);
        pFontData->pFontResource = pFont;

        pFontData->m_glyphTextures.reserve(FontData::kGlyphCount);

        // Create the Glyphs for the font.
        for(uint32_t i = 0; i < FontData::kGlyphCount; ++i)
        {
            auto* pGlyphTextureData = CreateGlyphTexture(i, pFont);
            if (!pGlyphTextureData)
            {
                pFontData->m_glyphTextures.emplace_back(nullptr);
                continue;
            }

            pFontData->m_glyphTextures.emplace_back(pGlyphTextureData);
        }

        return pFontData;
    }

    template <>
    void ResourceContainer<FontData, FontResourceRequest>::FreeResourceImpl(FontData* pFont)
    {
        TTF_CloseFont(static_cast<_TTF_Font*>(pFont->pFontResource));

        // Delete all of the created textures.
        for (auto* pTextureData : pFont->m_glyphTextures)
        {
            if (!pTextureData)
                continue;

            SDL_DestroyTexture(static_cast<SDL_Texture*>(pTextureData->pTexture));
            BLEACH_DELETE(pTextureData);
            pTextureData = nullptr;
        }

        BLEACH_DELETE(pFont);
    }
}
