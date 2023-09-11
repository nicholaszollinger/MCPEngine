// TiledTileSet.cpp

#include "TiledTileSet.h"

#include "MCP/Application/Debug/Log.h"
#include "MCP/Application/Resources/ResourceManager.h"
#include "Platform/TinyXML2/tinyxml2.h"

namespace mcp
{
    template <>
    template <>
    TiledTileSet* ResourceContainer<TiledTileSet>::LoadFromDiskImpl(const char* pFilePath, TiledTileSet::Properties& data, Texture& texture, const char*& pImageFilepath)
    {
        auto* pTileSet = BLEACH_NEW(TiledTileSet);

        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(pFilePath) != tinyxml2::XML_SUCCESS)
        {
            LogError("Failed to Load TileSetData! XML Error: %", tinyxml2::XMLDocument::ErrorIDToName(doc.ErrorID()));
            return nullptr;
        }

        auto* pTileSetAttribute = doc.FirstChildElement("tileset");

        if (!pTileSetAttribute)
        {
            LogError("Failed to Load TileSetData! XML Error: %", tinyxml2::XMLDocument::ErrorIDToName(doc.ErrorID()));
            return nullptr;
        }

        data.tileWidth = pTileSetAttribute->IntAttribute("tilewidth");
        data.tileHeight = pTileSetAttribute->IntAttribute("tileheight");
        data.tileCount = pTileSetAttribute->IntAttribute("tilecount");
        data.tileColumnCount = pTileSetAttribute->IntAttribute("columns");

        // Load the TileSet's image.
        texture.Load(pImageFilepath);

        return pTileSet;
    }

    template <>
    template <>
    TiledTileSet* ResourceContainer<TiledTileSet>::LoadFromRawDataImpl(char*, const int)
    {
        // TODO ?
        LogError("Failed to Load TileSetData! Load from Raw not implemented yet!");
        return nullptr;
    }

    template <>
    void ResourceContainer<TiledTileSet>::FreeResourceImpl(TiledTileSet* pTileSet)
    {
        BLEACH_DELETE(pTileSet); // Note: The Texture will be free as a consequence of the destruction of this image.
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Load the TileSet and its TileSetImage Resource.
    ///		@param pTileSetPath :
    ///		@param pTileSetImagePath :
    //-----------------------------------------------------------------------------------------------------------------------------
    bool TiledTileSet::Load(const char* pTileSetPath, const char* pTileSetImagePath)
    {
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(pTileSetPath) != tinyxml2::XML_SUCCESS)
        {
            LogError("Failed to Load TileSetData! XML Error: %", tinyxml2::XMLDocument::ErrorIDToName(doc.ErrorID()));
            return false;
        }

        auto* pTileSetAttribute = doc.FirstChildElement("tileset");

        if (!pTileSetAttribute)
        {
            LogError("Failed to Load TileSetData! XML Error: %", tinyxml2::XMLDocument::ErrorIDToName(doc.ErrorID()));
            return false;
        }

        m_props.tileWidth = pTileSetAttribute->IntAttribute("tilewidth");
        m_props.tileHeight = pTileSetAttribute->IntAttribute("tileheight");
        m_props.tileCount = pTileSetAttribute->IntAttribute("tilecount");
        m_props.tileColumnCount = pTileSetAttribute->IntAttribute("columns");

        // Load the TileSet's image.
        m_tileSetImage.Load(pTileSetImagePath);

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the renderData for a specific index. This basically sets the texture and the crop. The destination for the
    ///             TextureRenderData is NOT set. That is up to the Tiled map to figure out.
    ///		@param tileIndex : Index into the tile set that we want to get render data for
    //-----------------------------------------------------------------------------------------------------------------------------
    TextureRenderData TiledTileSet::GetRenderData(const size_t tileIndex) const
    {
        assert(tileIndex < m_props.tileCount);

        RectInt crop{0, 0, static_cast<int>(m_props.tileWidth), static_cast<int>(m_props.tileHeight)};

        crop.position.x = static_cast<int>(m_props.tileWidth * (tileIndex % m_props.tileColumnCount));
        crop.position.y  = static_cast<int>(m_props.tileHeight * (tileIndex / m_props.tileColumnCount));

        TextureRenderData renderData;
        renderData.pTexture = m_tileSetImage.Get();
        renderData.crop = crop;

        return renderData;
    }

}