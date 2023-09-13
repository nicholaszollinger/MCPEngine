// TiledMapComponent.cpp

#include "TiledMapComponent.h"

#include <BleachNew.h>
#include "MCP/Core/Resource/ResourceManager.h"
#include "MCP/Scene/Scene.h"
#include "Platform/TinyXML2/tinyxml2.h"

#include "MCP/Components/TransformComponent.h"

static std::string* GetPrefabPath(const tinyxml2::XMLElement* pProperty)
{
    static constexpr const char* kPrefabsFolder = "Assets/Prefabs/";
    static std::string prefabPath;
    prefabPath = kPrefabsFolder;

    const char* pPrefabPropName = pProperty->Attribute("name", "Prefab");
    if (!pPrefabPropName)
    {
        MCP_ERROR("TiledMapComponent", "Failed to get Prefab! No prefab property found!");
        return nullptr;
    }

    const char* pPrefab = pProperty->Attribute("value");
    prefabPath += pPrefab;

    return &prefabPath;

}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Specific buffer read for getting indexes from our text buffer.
///		@returns : The index value from the buffer.
//-----------------------------------------------------------------------------------------------------------------------------
static size_t GetNextIndex(const char* pBuffer, size_t& bufferIndex)
{
    std::string str;
    while(pBuffer[bufferIndex] != ',')
    {
        str += pBuffer[bufferIndex];
        ++bufferIndex;
    }

    // Move past the comma.
    ++bufferIndex;
    return std::stoull(str);
}

namespace mcp
{

    TiledMapComponent::TiledMapComponent(Object* pObject, const char* pMapPath, const char* pTileSetPath, const char* pTileSetImagePath, const float scale)
        : Component(pObject)
        , IRenderable(RenderLayer::kWorld, -10)
        , m_sizeScalar(scale)
    {
        Load(pMapPath, pTileSetPath, pTileSetImagePath);
    }

    TiledMapComponent::~TiledMapComponent()
    {
        // Remove the renderable from the scene.
        m_pOwner->GetScene()->RemoveRenderable(this);
    }

    void TiledMapComponent::Load(const char* pMapPath, const char* pTileSetPath, const char* pTileSetImagePath)
    {
        // Load the TileSet.
        if (!m_tileSet.Load(pTileSetPath, pTileSetImagePath))
        {
            MCP_ERROR("TiledMapComponent","Failed to load TiledMapComponent! Failed to Load TileSet!");
            return;
        }

        // Load the map.
        if (!LoadMapData(pMapPath))
        {
            MCP_ERROR("TiledMapComponent", "Failed to load TiledMapComponent! Failed to Load Map!");
            return;
        }
    }

    bool TiledMapComponent::Init()
    {
        // Add this component to the list of renderables in the scene.
        auto* pScene = m_pOwner->GetScene();
        assert(pScene);
        pScene->AddRenderable(this);

        m_pTransformComponent = m_pOwner->GetComponent<TransformComponent>();
        if (!m_pTransformComponent)
        {
            MCP_ERROR("TiledMapComponent", "Failed to initialize TiledMapComponent! TransformComponent was nullptr!");
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Render the map.
    //-----------------------------------------------------------------------------------------------------------------------------
    void TiledMapComponent::Render() const
    {
        if (m_mapSize == 0)
            return;

        // Get the the window dimensions.
        const auto windowDimensions = GraphicsManager::Get()->GetWindow()->GetDimensions();
        RectF windowRect = windowDimensions.GetRectAs<float>();
        windowRect.SetPosition(Vec2{});

        // Get the tileSet properties.
        const auto props = m_tileSet.GetTileSetProps();

        // Calculate the final tile width and final tile height size.
        const float finalTileWidth = static_cast<float>(props.tileWidth) * m_sizeScalar;
        const float finalTileHeight = static_cast<float>(props.tileHeight) * m_sizeScalar;

        const Vec2 currentLocation = m_pTransformComponent->GetLocation();

        // Calculate the offset to the left corner of the map.
        const Vec2 leftCornerPos{currentLocation.x - static_cast<float>(m_mapWidth) * finalTileWidth / 2,
           currentLocation.y - static_cast<float>(m_mapHeight) * finalTileHeight / 2};
        
        RectF destinationRect;
        destinationRect.width = finalTileWidth;
        destinationRect.height = finalTileHeight;

        // TODO: For each layer,
        for (size_t i = 0; i < m_mapSize; ++i)
        {
            // Calculate the destination rect m_position.
            destinationRect.x = destinationRect.width * (static_cast<float>(i % m_mapWidth));
            destinationRect.y = destinationRect.height * (static_cast<float>(i / m_mapWidth));

            // Offset the m_position of the rect up to the left corner of the map.
            destinationRect.SetPosition(leftCornerPos + destinationRect.GetPosition());

            // If the rect is not in the window space, then don't render it.
            // TODO: This needs to be the camera space.
            if (!windowRect.Intersects(destinationRect))
            {
                continue;
            }

            auto renderData = m_tileSet.GetRenderData(m_layers[0][i]);
            renderData.destinationRect = destinationRect;

            // Draw the tile.
            DrawTexture(renderData);
        }
    }

    bool TiledMapComponent::LoadMapData(const char* pMapFilepath)
    {
        // Open the .tmx file.
        tinyxml2::XMLDocument mapDoc;
        if (mapDoc.LoadFile(pMapFilepath) != tinyxml2::XML_SUCCESS)
        {
            MCP_ERROR("TiledMapComponent", "Failed to Load Map! XML Error: ", tinyxml2::XMLDocument::ErrorIDToName(mapDoc.ErrorID()));
            return false;
        }

        const auto* pMapElement = mapDoc.FirstChildElement("map");
        m_mapWidth = pMapElement->IntAttribute("width");
        m_mapHeight = pMapElement->IntAttribute("height");
        m_mapSize = static_cast<size_t>(m_mapWidth) * m_mapHeight;

        // Do I need this?
        //const auto* pTileSet = pMapElement->FirstChildElement("tileset");
        //const char* tileSetFilename = pTileSet->Attribute("source");

        // Load the tile layer data.
        auto* pLayer = pMapElement->FirstChildElement("layer");
        while(pLayer)
        {
            // create the layer of tiles.
            const auto* pData = pLayer->FirstChildElement("data");
            if (!pData)
            {
                MCP_ERROR("TiledMapComponent", "Failed to Load Tiled Texture! Failed to find layer data!");
                return false;
            }

            // Load all of the text into our 'buffer'
            auto* pLayerData = pData->GetText();
            GenerateLayerData(pLayerData);

            pLayer = pLayer->NextSiblingElement("layer");
        }

        // Map of prefabs that we are using to construct the scene.
        std::vector<tinyxml2::XMLDocument*> loadedPrefabs;
        loadedPrefabs.reserve(10);
        std::unordered_map<std::string, size_t> prefabIndexes;

        // Load the Object group data.
        auto* pObjectGroup = pMapElement->FirstChildElement("objectgroup");
        while(pObjectGroup)
        {
            // Get the first object:
            const auto* pObjectElement = pObjectGroup->FirstChildElement("object");

            while(pObjectElement)
            {
                // We need to get the correct prefab of the object.
                //std::string prefabPath = kPrefabsFolder;

                // Get the position and dimensions of the object.
                const float x = pObjectElement->FloatAttribute("x");
                const float y = pObjectElement->FloatAttribute("y");
                float width = -1.f;
                float height = -1.f;

                const bool isPoint = pObjectElement->FirstChildElement("point");
                // If we are not a 'point' we need to get the width and height too.
                if (!isPoint)
                {
                    width = pObjectElement->FloatAttribute("width");
                    height = pObjectElement->FloatAttribute("height");
                }

                // Get the prefab that we need to use to construct the object.
                auto* pProperties = pObjectElement->FirstChildElement("properties");
                if (!pProperties)
                {
                    MCP_ERROR("TiledMapComponent", "Failed to create Object from Tiled data! Failed to find Properties for Object!");
                    // Return?
                }

                // Technically, I could make another loop to get the properties that we need.
                // But I only have one.
                auto* pPrefabProp = pProperties->FirstChildElement("property");
                auto* pPrefabPath = GetPrefabPath(pPrefabProp);

                if (!pPrefabPath)
                {
                    MCP_ERROR("TiledMapComponent", "Failed to create Object from Tiled data! No prefab found for object!");
                    // Return?
                    return false;
                }

                auto result = prefabIndexes.find(*pPrefabPath);
                // If we don't have an open file for that prefab, then open it and save it in our map.
                if (result == prefabIndexes.end())
                {
                    // No copy or move constructor for the XMLDocument so I have to create a new one in the map,
                    // I can't even use emplace.
                    loadedPrefabs.emplace_back(BLEACH_NEW(tinyxml2::XMLDocument));
                    result = prefabIndexes.emplace(*pPrefabPath, loadedPrefabs.size() - 1).first;

                    // Load the prefab file.
                    if (loadedPrefabs.back()->LoadFile((*pPrefabPath).c_str()) != tinyxml2::XML_SUCCESS)
                    {
                        MCP_ERROR("TiledMapComponent", "Failed to Load Prefab! XML Error: ", tinyxml2::XMLDocument::ErrorIDToName(mapDoc.ErrorID()));

                        for (auto* pDoc : loadedPrefabs)
                        {
                            BLEACH_DELETE(pDoc);
                        }

                        return false;
                    }
                }

                // Create the object
                // Create a new object and add it to the scene.
                auto* pObject = m_pOwner->GetScene()->CreateObject();

                // Get the object header.
                auto* pObjectPrefabElement = loadedPrefabs[result->second]->FirstChildElement("Object");

                // Get the first Component of the object.
                auto* pComponentElement = pObjectPrefabElement->FirstChildElement();

                // Hack:
                std::string componentName;
                static const std::string transformName = "TransformComponent";
                static const std::string colliderName = "ColliderComponent";

                while (pComponentElement)
                {
                    componentName = pComponentElement->Value();
                    // Fill out the prefab data before adding it to the object.
                    // String compare :(
                    if (componentName == transformName)
                    {
                        if (width < 0.f)
                        {
                            pComponentElement->SetAttribute("x", x);
                            pComponentElement->SetAttribute("y", y);
                        }

                        else
                        {
                            pComponentElement->SetAttribute("x", (x + (width/2.f)));
                            pComponentElement->SetAttribute("y", (y + (height/2.f)));
                        }
                    }

                    else if (width > 0.f && componentName == colliderName)
                    {
                        auto* pBoxCollider = pComponentElement->FirstChildElement("Box2DCollider");
                        pBoxCollider->SetAttribute("width", width);
                        pBoxCollider->SetAttribute("height", height);
                    }

                    // Add the Component to the new Object.
                    if (!ComponentFactory::AddToObjectFromData(pComponentElement->Value(), pComponentElement, pObject))
                    {
                        delete pObject;
                        return false;
                    }

                    // Go to the next sibling to see if we have another component.
                    pComponentElement = pComponentElement->NextSiblingElement();
                }

                // Move to the next object
                pObjectElement = pObjectElement->NextSiblingElement("object");
            }

            // Move to the next object group.
            pObjectGroup = pObjectGroup->NextSiblingElement("objectgroup");
        }

        // Dismantle our prefabs.
        for (auto* pDoc : loadedPrefabs)
        {
            BLEACH_DELETE(pDoc);
        }

        return true;
    }

    void TiledMapComponent::GenerateLayerData(const char* pBuffer)
    {
        Layer& layer = m_layers.emplace_back();
        layer.reserve(m_mapSize);
        size_t bufferIndex = 0;

        for (size_t i = 0; i < m_mapHeight; ++i)
        {
            // Skip the newline char.
            ++bufferIndex;

            for(size_t ii = 0; ii < m_mapWidth; ++ii)
            {
                layer.emplace_back(GetNextIndex(pBuffer, bufferIndex) - 1); // We subtract 1 to make it a 0 based index.
            }
        }
    }

#ifdef MCP_DATA_PARSER_TINYXML2
    bool TiledMapComponent::AddFromData(const void* pFileData, Object* pOwner)
    {
        auto* pTiledMapComponent = static_cast<const tinyxml2::XMLElement*>(pFileData);

        // Map File Path
        const char* mapPath = pTiledMapComponent->Attribute("mapPath");
        if (!mapPath)
        {
            MCP_ERROR("TiledMapComponent","Failed to add TiledMapComponent from Data! Couldn't find mapPath Attribute!");
            return false;
        }

        // Tile Set File Path
        const char* tileSetPath = pTiledMapComponent->Attribute("tileSetPath");
        if (!tileSetPath)
        {
            MCP_ERROR("TiledMapComponent","Failed to add TiledMapComponent from Data! Couldn't find tileSetPath Attribute!");
            return false;
        }

        // Image File path
        const char* tileSetImagePath = pTiledMapComponent->Attribute("tileSetImagePath");
        if (!tileSetImagePath)
        {
            MCP_ERROR("TiledMapComponent", "Failed to add TiledMapComponent from Data! Couldn't find tileSetImagePath Attribute!");
            return false;
        }

        // Scale of the tiles
        const auto* pScaleAttribute = pTiledMapComponent->FirstChildElement("Scale");
        if (!pScaleAttribute)
        {
            MCP_ERROR("TiledMapComponent","Failed to add TiledMapComponent from Data! Couldn't find Scale Element!");
            return false;
        }

        const float scale = pScaleAttribute->FloatAttribute("scale");

        // Add the Component to the object
        if (!pOwner->AddComponent<TiledMapComponent>(mapPath, tileSetPath, tileSetImagePath, scale))
        {
            MCP_ERROR("TiledMapComponent","Failed to add TiledMapComponent from data!");
            return false;
        }

        return true;
    }

#else
    bool TiledMapComponent::AddFromData(const void*, Object*)
    {
        return false;
    }
#endif


}
