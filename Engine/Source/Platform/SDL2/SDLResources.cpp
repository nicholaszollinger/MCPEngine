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

namespace mcp
{
    //--------------------------------------------------------------------------------------------------------
    //      SDL_TEXTURES
    //--------------------------------------------------------------------------------------------------------

    template <>
    template <>
    SDL_Texture* ResourceContainer<SDL_Texture>::LoadFromDiskImpl(const char* pFilePath, Vec2Int& sizeOut)
    {
        SDL_Surface* pSurface = IMG_Load(pFilePath);

        if (!pSurface)
        {
            MCP_ERROR("SDL", "Failed to Load SDL_Surface at filepath: ", pFilePath, ". SDL_Error: ", SDL_GetError());
            return nullptr;
        }

        return CreateTextureFromSurface(pSurface, sizeOut);
    }

    /*template <>
    template <>
    SDL_Texture* ResourceContainer<SDL_Texture>::LoadFromDiskImpl(const char* pFilePath, const TextRenderData& data)
    {
        SDL_Surface* pSurface = TTF_RenderUTF8_Blended_Wrapped();

        return CreateTextureFromSurface(pSurface, )
    }*/


    template <>
    template <>
    SDL_Texture* ResourceContainer<SDL_Texture>::LoadFromRawDataImpl(char* pRawData, const int dataSize, Vec2Int& sizeOut)
    {
        SDL_RWops* pSdlData = SDL_RWFromMem(pRawData, dataSize);
        // TODO: Check for nullptr.

        SDL_Surface* pSurface = IMG_Load_RW(pSdlData, 0);

        if (!pSurface)
        {
            MCP_ERROR("SDL", "Failed to Load SDL_Surface from raw data! SDL_Error: ", SDL_GetError());
            return nullptr;
        }

        return CreateTextureFromSurface(pSurface, sizeOut);
    }

    template<>
    void ResourceContainer<SDL_Texture>::FreeResourceImpl(SDL_Texture* pTexture)
    {
        SDL_DestroyTexture(pTexture);
    }

    //--------------------------------------------------------------------------------------------------------
    //      MIX_CHUNK
    //--------------------------------------------------------------------------------------------------------

    template <>
    template <>
    Mix_Chunk* ResourceContainer<Mix_Chunk>::LoadFromDiskImpl(const char* pFilePath)
    {
        auto* pChunk = Mix_LoadWAV(pFilePath);
        if (!pChunk)
        {
            MCP_ERROR("SDL", "Failed to load Mix_Chunk at filepath: ", pFilePath);
            return nullptr;
        }

        return pChunk;
    }

    template <>
    template <>
    Mix_Chunk* ResourceContainer<Mix_Chunk>::LoadFromRawDataImpl(char* pRawData, const int dataSize)
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
    void ResourceContainer<Mix_Chunk>::FreeResourceImpl(Mix_Chunk* pChunk)
    {
        Mix_FreeChunk(pChunk);
    }

    //--------------------------------------------------------------------------------------------------------
    //      MIX_MUSIC
    //--------------------------------------------------------------------------------------------------------

    template <>
    template <>
    Mix_Music* ResourceContainer<Mix_Music>::LoadFromDiskImpl(const char* pFilePath)
    {
        auto* pMusic = Mix_LoadMUS(pFilePath);
        if (!pMusic)
        {
            MCP_ERROR("SDL", "Failed to load Mix_Music at filepath: ", pFilePath);
            return nullptr;
        }

        return pMusic;
    }

    template <>
    template <>
    Mix_Music* ResourceContainer<Mix_Music>::LoadFromRawDataImpl(char* pRawData, const int dataSize)
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
    void ResourceContainer<Mix_Music>::FreeResourceImpl(Mix_Music* pFont)
    {
        Mix_FreeMusic(pFont);
    }

    //--------------------------------------------------------------------------------------------------------
    //      TTF_FONTS
    //--------------------------------------------------------------------------------------------------------

    template <>
    template <>
    _TTF_Font* ResourceContainer<_TTF_Font>::LoadFromDiskImpl(const char* pFilePath, int& fontSize)
    {
        auto* pFont = TTF_OpenFont(pFilePath, fontSize);

        if (!pFont)
        {
            MCP_ERROR("SDL", "Failed to load TTF_Font from data! TTF_Error: ", TTF_GetError());
            return nullptr;
        }

        return pFont;
    }

    template <>
    template <>
    _TTF_Font* ResourceContainer<_TTF_Font>::LoadFromRawDataImpl(char* pRawData, const int dataSize, int& fontSize)
    {
        SDL_RWops* pSdlData = SDL_RWFromMem(pRawData, dataSize);
        auto* pFont = TTF_OpenFontRW(pSdlData, 0, fontSize);
        if (!pFont)
        {
            MCP_ERROR("SDL", "Failed to load TTF_Font from raw data! TTF_Error: ", TTF_GetError());
            return nullptr;
        }

        return pFont;
    }

    template <>
    void ResourceContainer<_TTF_Font>::FreeResourceImpl(TTF_Font* pFont)
    {
        TTF_CloseFont(pFont);
    }
}