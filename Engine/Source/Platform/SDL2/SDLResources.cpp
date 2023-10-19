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

    //template <>
    //template <>
    //SDL_Texture* ResourceContainer<SDL_Texture>::LoadFromDiskImpl(const char* pFilePath, Vec2Int& sizeOut)
    //{
    //    SDL_Surface* pSurface = IMG_Load(pFilePath);

    //    if (!pSurface)
    //    {
    //        MCP_ERROR("SDL", "Failed to Load SDL_Surface at filepath: ", pFilePath, ". SDL_Error: ", SDL_GetError());
    //        return nullptr;
    //    }

    //    return CreateTextureFromSurface(pSurface, sizeOut);
    //}

    //template <>
    //template <>
    //SDL_Texture* ResourceContainer<SDL_Texture>::LoadFromRawDataImpl(char* pRawData, const int dataSize, Vec2Int& sizeOut)
    //{
    //    SDL_RWops* pSdlData = SDL_RWFromMem(pRawData, dataSize);
    //    // TODO: Check for nullptr.

    //    SDL_Surface* pSurface = IMG_Load_RW(pSdlData, 0);

    //    if (!pSurface)
    //    {
    //        MCP_ERROR("SDL", "Failed to Load SDL_Surface from raw data! SDL_Error: ", SDL_GetError());
    //        return nullptr;
    //    }

    //    return CreateTextureFromSurface(pSurface, sizeOut);
    //}

    //template<>
    //void ResourceContainer<SDL_Texture>::FreeResourceImpl(SDL_Texture* pTexture)
    //{
    //    SDL_DestroyTexture(pTexture);
    //}
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

    template <>
    _TTF_Font* ResourceContainer<_TTF_Font, FontResourceRequest>::LoadFromDiskImpl(const FontResourceRequest& request)
    {
        auto* pFont = TTF_OpenFont(request.path.GetCStr(), request.fontSize);

        if (!pFont)
        {
            MCP_ERROR("SDL", "Failed to load TTF_Font from data! TTF_Error: ", TTF_GetError());
            return nullptr;
        }

        return pFont;
    }

    template <>
    _TTF_Font* ResourceContainer<_TTF_Font, FontResourceRequest>::LoadFromRawDataImpl(char* pRawData, const int dataSize, const FontResourceRequest& request)
    {
        SDL_RWops* pSdlData = SDL_RWFromMem(pRawData, dataSize);
        auto* pFont = TTF_OpenFontRW(pSdlData, 0, request.fontSize);
        if (!pFont)
        {
            MCP_ERROR("SDL", "Failed to load TTF_Font from raw data! TTF_Error: ", TTF_GetError());
            return nullptr;
        }

        return pFont;
    }

    template <>
    void ResourceContainer<_TTF_Font, FontResourceRequest>::FreeResourceImpl(TTF_Font* pFont)
    {
        TTF_CloseFont(pFont);
    }
}
