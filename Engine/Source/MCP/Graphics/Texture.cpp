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

    Texture::Texture(const Texture& right)
    {
        m_request = right.m_request;

        if (m_request.path.IsValid())
            m_pResource = LoadResourceType();
    }

    Texture::Texture(Texture&& right) noexcept
    {
        m_request = right.m_request;
        m_pResource = right.m_pResource;

        right.m_pResource = nullptr;
    }

    Texture& Texture::operator=(const Texture& right)
    {
        if (&right != this)
        {
            if (m_pResource)
                Free();

            m_request = right.m_request;

            if (m_request.path.IsValid())
                m_pResource = LoadResourceType();
        }

        return *this;
    }

    Texture& Texture::operator=(Texture&& right) noexcept
    {
        if (&right != this)
        {
            if (m_pResource)
                Free();

            m_request = right.m_request;
            m_pResource = right.m_pResource;

            right.m_pResource = nullptr;
        }
        
        return *this;
    }

    void* Texture::LoadResourceType()
    {
        return ResourceManager::Get()->LoadFromDisk<TextureData>(m_request);
    }

    void Texture::Free()
    {
        if (m_request.path.IsValid())
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