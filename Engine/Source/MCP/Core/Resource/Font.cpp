// Font.cpp

#include "Font.h"

#include "MCP/Core/Resource/ResourceManager.h"

#if MCP_RENDERER_API == MCP_RENDERER_API_SDL
#include "Platform/SDL2/SDLText.h"

namespace mcp
{
    using FontAssetType = _TTF_Font;
}
#else
#error "We don't have a resource implementation for Texture loading for current API!'"
#endif

namespace mcp
{
    Font::~Font()
    {
        if (m_pResource)
            Free();
    }

    void Font::Load(const char* pFilepath, const int fontSize, const char* pPackageName, const bool isPersistent)
    {
        m_fontSize = fontSize;
        Load(pFilepath, pPackageName, isPersistent);
    }

    void Font::SetSize(const int size) const
    {
        // This is pretty SDL specific in the way it is done. This may change later.
        SetFontSize(static_cast<FontAssetType*>(m_pResource), size);
    }

    void Font::Load(const char* pFilePath, const char* pPackageName, const bool isPersistent)
    {
        if (m_pResource)
            Free();

        m_loadData.pFilePath = pFilePath;
        m_loadData.pPackageName = pPackageName;
        m_loadData.isPersistent = isPersistent;

        m_pResource = ResourceManager::Get()->Load<FontAssetType>(m_loadData, m_fontSize);
    }

    void Font::Free()
    {
        ResourceManager::Get()->FreeResource<FontAssetType>(m_loadData.pFilePath);
    }
}