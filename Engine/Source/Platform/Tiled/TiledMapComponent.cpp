// TiledMapComponent.cpp

#include "TiledMapComponent.h"

#include "MCP/Core/Resource/ResourceManager.h"
#include "MCP/Scene/Scene.h"
#include "MCP/Components/TransformComponent.h"

static std::string* GetPrefabPath(const mcp::XMLElement element)
{
    static constexpr const char* kPrefabsFolder = "Assets/Prefabs/";
    static std::string prefabPath;
    prefabPath = kPrefabsFolder;
    
    //const char* pPrefabPropName = element.GetAttribute<const char*>("name", success);
    /*if (!success)
    {
        MCP_ERROR("TiledMapComponent", "Failed to get Prefab! No prefab property found!");
        return nullptr;
    }*/

    const char* pPrefab = element.GetAttribute<const char*>("value");
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
        XMLParser parser;

        if (!parser.LoadFile(pMapFilepath))
        {
            MCP_ERROR("TiledMapComponent", "Failed to Load Map!");
            return false;
        }

        bool success = true;
        const XMLElement mapElement = parser.GetElement("map");
        m_mapWidth = mapElement.GetAttribute<int>("width", success);
        m_mapHeight = mapElement.GetAttribute<int>("height", success);
        m_mapSize = static_cast<size_t>(m_mapWidth) * m_mapHeight;

        // Do I need this?
        //const auto* pTileSet = pMapElement->FirstChildElement("tileset");
        //const char* tileSetFilename = pTileSet->Attribute("source");

        // Load the tile layer data.
        XMLElement layer = mapElement.GetChildElement("layer");
        while(layer.IsValid())
        {
            // create the layer of tiles.
            const XMLElement data = layer.GetChildElement("data");
            if (!data.IsValid())
            {
                MCP_ERROR("TiledMapComponent", "Failed to Load Tiled Texture! Failed to find layer data!");
                return false;
            }

            // Load all of the text into our 'buffer'
            auto* pLayerData = data.GetText();
            GenerateLayerData(pLayerData);

            layer = layer.GetSiblingElement("layer");
        }

        // Map of prefabs that we are using to construct the scene.
        std::vector<XMLParser> loadedPrefabs;
        loadedPrefabs.reserve(10);
        std::unordered_map<std::string, size_t> prefabIndexes;

        // Load the Object group data.
        XMLElement objectGroup = mapElement.GetChildElement("objectgroup");
        while(objectGroup.IsValid())
        {
            // Get the first object:
            XMLElement objectElement = objectGroup.GetChildElement("object");

            while(objectElement.IsValid())
            {
                // We need to get the correct prefab of the object.
                //std::string prefabPath = kPrefabsFolder;

                // Get the position and dimensions of the object.
                const auto x = objectElement.GetAttribute<float>("x", success);
                const auto y = objectElement.GetAttribute<float>("y", success);
                float width = -1.f;
                float height = -1.f;

                const bool isPoint = objectElement.GetChildElement("point").IsValid();
                // If we are not a 'point' we need to get the width and height too.
                if (!isPoint)
                {
                    width = objectElement.GetAttribute<float>("width", success);
                    height = objectElement.GetAttribute<float>("height", success);
                }

                // Get the prefab that we need to use to construct the object.
                XMLElement properties = objectElement.GetChildElement("properties");
                if (!properties.IsValid())
                {
                    MCP_ERROR("TiledMapComponent", "Failed to create Object from Tiled data! Failed to find Properties for Object!");
                    // Return?
                }

                // Technically, I could make another loop to get the properties that we need.
                // But I only have one.
                const XMLElement prefabProp = properties.GetChildElement("property");
                auto* pPrefabPath = GetPrefabPath(prefabProp);

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
                    loadedPrefabs.emplace_back();
                    result = prefabIndexes.emplace(*pPrefabPath, loadedPrefabs.size() - 1).first;

                    // Load the prefab file.
                    if (!loadedPrefabs.back().LoadFile((*pPrefabPath).c_str()))
                    {
                        MCP_ERROR("TiledMapComponent", "Failed to Load Prefab!");

                        for (auto& prefabParser : loadedPrefabs)
                        {
                            prefabParser.CloseCurrentFile();
                        }

                        return false;
                    }
                }

                // Create the object
                // Create a new object and add it to the scene.
                auto* pObject = m_pOwner->GetScene()->CreateObject();

                // Get the object header.
                auto objectPrefabElement = loadedPrefabs[result->second].GetElement("Object");

                // Get the first Component of the object.
                auto componentElement = objectPrefabElement.GetChildElement();

                // Hack:
                std::string componentName;
                static const std::string transformName = "TransformComponent";
                static const std::string colliderName = "ColliderComponent";

                while (componentElement.IsValid())
                {
                    componentName = componentElement.GetName();
                    // Fill out the prefab data before adding it to the object.
                    // String compare :(
                    if (componentName == transformName)
                    {
                        if (width < 0.f)
                        {
                            componentElement.SetAttribute("x", x);
                            componentElement.SetAttribute("y", y);
                        }

                        else
                        {
                            componentElement.SetAttribute("x", (x + (width/2.f)));
                            componentElement.SetAttribute("y", (y + (height/2.f)));
                        }
                    }

                    else if (width > 0.f && componentName == colliderName)
                    {
                        XMLElement boxCollider = componentElement.GetChildElement("Box2DCollider");
                        boxCollider.SetAttribute("width", width);
                        boxCollider.SetAttribute("height", height);
                    }

                    // Add the Component to the new Object.
                    if (!ComponentFactory::AddToObjectFromData(componentElement.GetName(), componentElement, pObject))
                    {
                        delete pObject;
                        return false;
                    }

                    // Go to the next sibling to see if we have another component.
                    componentElement = componentElement.GetSiblingElement();
                }

                // Move to the next object
                objectElement = objectElement.GetSiblingElement("object");
            }

            // Move to the next object group.
            objectGroup = objectGroup.GetSiblingElement("objectgroup");
        }

        // Dismantle our prefabs.
        for (auto& prefabParser : loadedPrefabs)
        {
            prefabParser.CloseCurrentFile();
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
    
    bool TiledMapComponent::AddFromData(const XMLElement component, Object* pOwner)
    {
        // Map File Path
        const char* pMapPath = component.GetAttribute<const char*>("mapPath");
        if (!pMapPath)
        {
            MCP_ERROR("TiledMapComponent","Failed to add TiledMapComponent from Data! Couldn't find mapPath Attribute!");
            return false;
        }

        // Tile Set File Path
        const char* pTileSetPath = component.GetAttribute<const char*>("tileSetPath");
        if (!pTileSetPath)
        {
            MCP_ERROR("TiledMapComponent","Failed to add TiledMapComponent from Data! Couldn't find tileSetPath Attribute!");
            return false;
        }

        // Image File path
        const char* pTileSetImagePath = component.GetAttribute<const char*>("tileSetImagePath");
        if (!pTileSetImagePath)
        {
            MCP_ERROR("TiledMapComponent", "Failed to add TiledMapComponent from Data! Couldn't find tileSetImagePath Attribute!");
            return false;
        }

        // Scale of the tiles
        const XMLElement scaleElement = component.GetChildElement("Scale");
        if (!scaleElement.IsValid())
        {
            MCP_ERROR("TiledMapComponent","Failed to add TiledMapComponent from Data! Couldn't find Scale Element!");
            return false;
        }

        const float scale = scaleElement.GetAttribute<float>("scale");

        // Add the Component to the object
        if (!pOwner->AddComponent<TiledMapComponent>(pMapPath, pTileSetPath, pTileSetImagePath, scale))
        {
            MCP_ERROR("TiledMapComponent","Failed to add TiledMapComponent from data!");
            return false;
        }

        return true;
    }

}
