// Texture.cpp

#include "Texture.h"

#include "MCP/Core/Resource/ResourceManager.h"

#if MCP_RENDERER_API == MCP_RENDERER_API_SDL

struct SDL_Texture;

namespace mcp
{
    using TextureAssetType = SDL_Texture;
}
#else
#error "We don't have a resource implementation for Texture loading for current API!'"
#endif

namespace mcp
{
    TextureData::TextureData(void* pTexture, const int width, const int height)
        : pTexture(pTexture)
        , width(width)
        , height(height)
    {
        //
    }


    //Texture::~Texture()
    //{
    //    if (m_pResource)
    //    {
    //        Free();
    //    }
    //}

    //void Texture::Load(const char* pFilePath, const char* pPackageName, const bool isPersistent)
    //{
    //    if (m_pResource)
    //    {
    //        Free();
    //    }

    //    //m_loadData.pFilePath = pFilePath;
    //    //m_loadData.isPersistent = isPersistent;
    //    //m_loadData.pPackageName = pPackageName;
    //    m_request.path = pFilePath;
    //    m_request.packagePath = pPackageName;
    //    m_request.isPersistent = isPersistent;

    //    m_pResource = ResourceManager::Get()->LoadFromDisk<TextureData>(m_request);
    //}

    /*bool Texture::Load(const DiskResourceRequest& request)
    {
        m_request = request;

        m_pResource = ResourceManager::Get()->LoadFromDisk<TextureData>(m_request);
        return m_pResource != nullptr;
    }*/

    void* Texture::LoadResourceType()
    {
        return ResourceManager::Get()->LoadFromDisk<TextureData>(m_request);
    }


    void Texture::Free()
    {
        ResourceManager::Get()->FreeResource<TextureData>(m_request);
    }

    void* Texture::Get() const
    {
        return static_cast<TextureData*>(m_pResource)->pTexture;
    }

    Vec2Int Texture::GetTextureSize() const
    {
        if (!m_pResource)
        {
            MCP_WARN("Texutre", "Attempting to get size of invalid texture resource.");
            return {};
        }

        auto* pTextureData = static_cast<TextureData*>(m_pResource);
        return {pTextureData->width, pTextureData->height};
    }

    Vec2 Texture::GetTextureSizeAsVec2() const
    {
        const auto vec = GetTextureSize();
        return {static_cast<float>(vec.x), static_cast<float>(vec.y)};
    }
}