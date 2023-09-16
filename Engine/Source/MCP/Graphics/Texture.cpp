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
    Texture::~Texture()
    {
        if (m_pResource)
        {
            Free();
        }
    }

    void Texture::Load(const char* pFilePath, const char* pPackageName, const bool isPersistent)
    {
        if (m_pResource)
        {
            Free();
        }

        m_loadData.pFilePath = pFilePath;
        m_loadData.isPersistent = isPersistent;
        m_loadData.pPackageName = pPackageName;

        m_pResource = ResourceManager::Get()->Load<TextureAssetType>(m_loadData, m_baseSize);
    }

    void Texture::Free()
    {
        ResourceManager::Get()->FreeResource<TextureAssetType>(m_loadData.pFilePath);
    }

}