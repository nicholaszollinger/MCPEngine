// ImageComponent.h

#include "ImageComponent.h"

#include "TransformComponent.h"
#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"
#include "MCP/Graphics/Graphics.h"

#ifdef MCP_DATA_PARSER_TINYXML2
#include "Platform/TinyXML2/tinyxml2.h"
#else
#error "ImageComponent does not have support a the currently defined Data Parser!"
#endif

namespace mcp
{
    ImageComponent::ImageComponent(Object* pObject, const RenderLayer layer, const int zOrder)
        : Component(pObject)
        , IRenderable(layer, zOrder)
        , m_pTransformComponent(nullptr)
        , m_renderAngle(0.0)
        , m_flip(RenderFlip2D::kNone)
    {
        m_size = m_texture.GetBaseSizeAsFloat();
    }

    ImageComponent::ImageComponent(Object* pObject, const char* pTextureFilepath, const RectInt& crop, const Vec2 size, const RenderLayer layer, const int zOrder)
        : Component(pObject)
        , IRenderable(layer, zOrder)
        , m_pTransformComponent(nullptr)
        , m_crop(crop)
        , m_size(size)
        , m_renderAngle(0.0)
        , m_flip(RenderFlip2D::kNone)
    {
        m_texture.Load(pTextureFilepath);
    }

    ImageComponent::~ImageComponent()
    {
        auto* pScene = m_pOwner->GetScene();
        assert(pScene);
        pScene->RemoveRenderable(this);
    }

    bool ImageComponent::Init()
    {
        // Add this component to the list of renderables in the scene.
        auto* pScene = m_pOwner->GetScene();
        assert(pScene);
        pScene->AddRenderable(this);

        m_pTransformComponent = m_pOwner->GetComponent<TransformComponent>();
        if (!m_pTransformComponent)
        {
            LogError("Failed to initialize ImageComponent! TransformComponent was nullptr!");
            return false;
        }

        return true;
    }

    void ImageComponent::Render() const
    {
        assert(m_pTransformComponent && "Failed to Render ImageComponent! TransformComponent was nullptr!");

        const Vec2 location = m_pTransformComponent->GetLocation();
        const float renderXPos = location.x - m_size.x / 2.f;
        const float renderYPos = location.y - m_size.y / 2.f;

        const RectF destinationRect {renderXPos, renderYPos, m_size.x, m_size.y};

        TextureRenderData renderData;
        renderData.pTexture = m_texture.Get();
        renderData.angle = m_renderAngle;
        renderData.crop = m_crop;
        renderData.anglePivot = m_anglePivot;
        renderData.destinationRect = destinationRect;
        renderData.flip = m_flip;

        DrawTexture(renderData);
    }

#ifdef MCP_DATA_PARSER_TINYXML2
    bool ImageComponent::AddFromData(const void* pFileData, Object* pOwner)
    {
        auto* pImageComponentElement = static_cast<const tinyxml2::XMLElement*>(pFileData);

        // File Path
        const char* pTextureFilePath = pImageComponentElement->Attribute("imagePath");
        if (!pTextureFilePath)
        {
            LogError("Failed to add ImageComponent from Data! Couldn't find imagePath Attribute!");
            return false;
        }

        // Crop
        RectInt crop;
        const auto* pCrop = pImageComponentElement->FirstChildElement("Crop");
        if (!pCrop)
        {
            LogError("Failed to add ImageComponent from Data! Couldn't find Crop Attribute!");
            return false;
        }

        crop.position.x = pCrop->IntAttribute("x");
        crop.position.y = pCrop->IntAttribute("y");
        crop.width = pCrop->IntAttribute("w");
        crop.height = pCrop->IntAttribute("h");

        // Size
        Vec2 size;
        const auto* pSize = pCrop->NextSiblingElement("Size");
        if (!pSize)
        {
            LogError("Failed to add ImageComponent from Data! Couldn't find Size Attribute!");
            return false;
        }

        size.x = pSize->FloatAttribute("x");
        size.y = pSize->FloatAttribute("y");

        // IRenderable Data.
        const auto* pRenderable = pCrop->NextSiblingElement("Renderable");
        const RenderLayer layer = static_cast<RenderLayer>(pRenderable->IntAttribute("layer"));
        const int zOrder = pRenderable->IntAttribute("zOrder");

        // Add the component to the Object
        if (!pOwner->AddComponent<ImageComponent>(pTextureFilePath, crop, size, layer, zOrder))
        {
            LogError("Failed to add ImageComponent from data!");
            return false;
        }

        return true;
    }

#else
    bool ImageComponent::AddFromData(const void*, Object*)
    {
        return false;
    }
#endif

}