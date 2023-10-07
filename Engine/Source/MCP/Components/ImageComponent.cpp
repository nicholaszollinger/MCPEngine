// ImageComponent.h

#include "ImageComponent.h"

#include "TransformComponent.h"
#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"
#include "MCP/Graphics/Graphics.h"

namespace mcp
{
    ImageComponent::ImageComponent(Object* pObject, const RenderLayer layer, const int zOrder)
        : Component(pObject)
        , IRenderable(layer, zOrder)
        , m_pTransformComponent(nullptr)
        , m_crop()
        , m_tint(Color::White())
        , m_renderAngle(0.0)
        , m_flip(RenderFlip2D::kNone)
    {
        m_size = m_texture.GetBaseSizeAsFloat();
    }

    ImageComponent::ImageComponent(Object* pObject, const char* pTextureFilepath, const RectInt& crop, const Vec2 size, const Color color, const RenderLayer layer, const int zOrder)
        : Component(pObject)
        , IRenderable(layer, zOrder)
        , m_pTransformComponent(nullptr)
        , m_crop(crop)
        , m_size(size)
        , m_tint(color)
        , m_renderAngle(0.0)
        , m_flip(RenderFlip2D::kNone)
    {
        m_texture.Load(pTextureFilepath);
    }

    ImageComponent::~ImageComponent()
    {
        auto* pWorld = m_pOwner->GetWorld();
        assert(pWorld);
        pWorld->RemoveRenderable(this);
    }

    bool ImageComponent::Init()
    {
        // Add this component to the list of renderables in the scene.
        auto* pWorld = m_pOwner->GetWorld();
        assert(pWorld);
        pWorld->AddRenderable(this);

        m_pTransformComponent = m_pOwner->GetComponent<TransformComponent>();
        if (!m_pTransformComponent)
        {
            MCP_ERROR("ImageComponent", "Failed to initialize ImageComponent! TransformComponent was nullptr!");
            return false;
        }

        return true;
    }

    void ImageComponent::SetIsActive(const bool isActive)
    {
        if (isActive == m_isActive)
            return;

        // If we are being set to active:
        if (isActive)
        {
            m_pOwner->GetWorld()->AddRenderable(this);
        }

        // If we are being set to inactive.
        else
        {
            m_pOwner->GetWorld()->RemoveRenderable(this);
        }

        m_isActive = isActive;
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
        renderData.tint = m_tint;
        renderData.anglePivot = m_anglePivot;
        renderData.destinationRect = destinationRect;
        renderData.flip = m_flip;

        DrawTexture(renderData);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Sets the color and alpha value of the tint of the texture.
    //-----------------------------------------------------------------------------------------------------------------------------
    void ImageComponent::SetTint(const Color color)
    {
        m_tint = color;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Sets the alpha of the texture.
    //-----------------------------------------------------------------------------------------------------------------------------
    void ImageComponent::SetAlpha(const uint8_t alpha)
    {
        m_tint.alpha = alpha;
    }
    
    bool ImageComponent::AddFromData(const XMLElement component, Object* pOwner)
    {
        // File Path
        const char* pTextureFilePath = component.GetAttributeValue<const char*>("imagePath");

        if (!pTextureFilePath)
        {
            MCP_ERROR("ImageComponent","Failed to add ImageComponent from Data! Couldn't find imagePath Attribute!");
            return false;
        }

        // Crop
        RectInt crop;
        const XMLElement cropElement = component.GetChildElement("Crop");
        if (!cropElement.IsValid())
        {
            MCP_ERROR("ImageComponent","Failed to add ImageComponent from Data! Couldn't find Crop Attribute!");
            return false;
        }
        
        crop.x = cropElement.GetAttributeValue<int>("x");
        crop.y = cropElement.GetAttributeValue<int>("y");
        crop.width = cropElement.GetAttributeValue<int>("w");
        crop.height = cropElement.GetAttributeValue<int>("h");

        // Size
        Vec2 size;
        const XMLElement sizeElement = cropElement.GetSiblingElement("Size");
        if (!sizeElement.IsValid())
        {
            MCP_ERROR("ImageComponent","Failed to add ImageComponent from Data! Couldn't find Size Attribute!");
            return false;
        }

        size.x = sizeElement.GetAttributeValue<float>("x");
        size.y = sizeElement.GetAttributeValue<float>("y");

        Color color = Color::White();
        const XMLElement tintElement = cropElement.GetSiblingElement("Color");
        if (tintElement.IsValid())
        {
            color.r = tintElement.GetAttributeValue<uint8_t>("r");
            color.g = tintElement.GetAttributeValue<uint8_t>("g");
            color.b = tintElement.GetAttributeValue<uint8_t>("b");
            color.alpha = tintElement.GetAttributeValue<uint8_t>("alpha");
        }

        // IRenderable Data.
        const XMLElement renderableElement = cropElement.GetSiblingElement("Renderable");
        
        const auto layer =static_cast<RenderLayer>(renderableElement.GetAttributeValue<int>("layer"));
        const int zOrder = renderableElement.GetAttributeValue<int>("zOrder");

        // Add the component to the Object
        if (!pOwner->AddComponent<ImageComponent>(pTextureFilePath, crop, size, color, layer, zOrder))
        {
            MCP_ERROR("ImageComponent","Failed to add ImageComponent from data!");
            return false;
        }

        return true;
    }
}