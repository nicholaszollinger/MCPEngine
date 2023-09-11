#pragma once
// TiledMapComponent.h

#include <vector>
#include "TiledTileSet.h"
#include "MCP/Components/Component.h"
#include "MCP/Scene/IRenderable.h"

namespace mcp
{
    class TransformComponent;

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      I am going to make some assumptions:
    //          - Single Layer.
    //          - File Storage. I might have to make a build script that will format this for me. It could make a new file for me to read
    //            with all of the file paths that I need in one place. Like TiledMapData.tiled or something.
    //		
    ///		@brief : 
    //-----------------------------------------------------------------------------------------------------------------------------
    class TiledMapComponent final : public Component, public IRenderable
    {
        MCP_DEFINE_COMPONENT_ID(TiledMapComponent)

        using Layer = std::vector<size_t>;

        static constexpr const char* kTileSetFileExtension = "tsx";
        static constexpr const char* kTileMapFileExtension = "tmx";
        static constexpr const char* kTiledTemplateFolder = "/Assets/Tiled/Templates/";
        static constexpr const char* kPrefabsFolder = "/Assets/Prefabs/";

        std::vector<Layer> m_layers;
        TiledTileSet m_tileSet;
        TransformComponent* m_pTransformComponent = nullptr;
        size_t m_mapSize            = 0;
        uint32_t m_mapWidth         = 0;
        uint32_t m_mapHeight        = 0;
        float m_sizeScalar          = 1.f;

    public:
        TiledMapComponent(Object* pObject, const char* pMapPath, const char* pTileSetPath, const char* pTileSetImagePath, const float scale = 1.f);
        virtual ~TiledMapComponent() override;

        virtual bool Init() override;
        virtual void Render() const override;

        //RectInt GetMapDimensions();
        static bool AddFromData(const void* pFileData, Object* pOwner);

    private:
        void Load(const char* pMapPath, const char* pTileSetPath, const char* pTileSetImagePath);
        bool LoadMapData(const char* pMapFilepath);
        void GenerateLayerData(const char* pBuffer);
    };
}

