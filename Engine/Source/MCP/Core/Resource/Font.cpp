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
    void Font::SetSize(const int size) const
    {
        // This is pretty SDL specific in the way it is done. This may change later.
        SetFontSize(static_cast<FontAssetType*>(m_pResource), size);
    }

    void* Font::LoadResourceType()
    {
        return ResourceManager::Get()->LoadFromDisk<FontAssetType>(m_request);
    }

    void Font::Free()
    {
        ResourceManager::Get()->FreeResource<FontAssetType>(m_request);
    }
}