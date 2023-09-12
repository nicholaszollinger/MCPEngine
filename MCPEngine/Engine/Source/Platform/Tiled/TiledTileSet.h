#pragma once

#include "MCP/Graphics/Graphics.h"
#include "MCP/Graphics/Texture.h"

namespace mcp
{
    class TiledTileSet
    {
    public:
        struct Properties
        {
            uint32_t tileCount        = 0;
            uint32_t tileColumnCount  = 0;
            uint32_t tileWidth        = 0;
            uint32_t tileHeight       = 0;
        };

    private:
        Texture m_tileSetImage;
        Properties m_props;

    public:
        [[nodiscard]] bool Load(const char* pTileSetPath, const char* pTileSetImagePath);

        [[nodiscard]] TextureRenderData GetRenderData(const size_t tileIndex) const;
        [[nodiscard]] const Properties& GetTileSetProps() const { return m_props; }
    };
    
}
