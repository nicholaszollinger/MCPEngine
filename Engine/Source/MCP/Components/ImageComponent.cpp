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
        //m_scale = m_texture.GetTextureSizeAsVec2();
    }

    ImageComponent::ImageComponent(Object* pObject, const char* pTextureFilepath, const RectInt& crop, const Vec2 scale, const Color color, const RenderLayer layer, const int zOrder)
        : Component(pObject)
        , IRenderable(layer, zOrder)
        , m_pTransformComponent(nullptr)
        , m_crop(crop)
        , m_scale(scale)
        , m_tint(color)
        , m_renderAngle(0.0)
        , m_flip(RenderFlip2D::kNone)
    {
        if (!m_texture.Load({pTextureFilepath, nullptr, false}))
        {
            MCP_WARN("ImageComponent", "Failed to load texture!");
        }
    }

    ImageComponent::~ImageComponent()
    {
        if (m_isActive)
        {
            auto* pWorld = m_pOwner->GetWorld();
            MCP_CHECK(pWorld);
            pWorld->RemoveRenderable(this);
        }
    }

    bool ImageComponent::Init()
    {
        // Add this component to the list of renderables in the scene.
        auto* pWorld = m_pOwner->GetWorld();
        MCP_CHECK(pWorld);

        if (m_texture.IsValid())
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
        MCP_CHECK_MSG(m_pTransformComponent, "Failed to Render ImageComponent! TransformComponent was nullptr!");

        const float width = m_scale.x * static_cast<float>(m_crop.width);
        const float height = m_scale.y * static_cast<float>(m_crop.height);

        const Vec2 location = m_pTransformComponent->GetLocation();
        const float renderXPos = location.x - (width / 2.f);
        const float renderYPos = location.y - (height / 2.f);
        const RectF destinationRect
        {
            renderXPos
            , renderYPos
            , width
            , height
        };

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

        // Scale
        Vec2 scale = {1.f, 1.f};
        const XMLElement scaleElement = cropElement.GetSiblingElement("Scale");
        if (scaleElement.IsValid())
        {
            scale.x = scaleElement.GetAttributeValue<float>("x");
            scale.y = scaleElement.GetAttributeValue<float>("y");
        }

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
        if (!pOwner->AddComponent<ImageComponent>(pTextureFilePath, crop, scale, color, layer, zOrder))
        {
            MCP_ERROR("ImageComponent","Failed to add ImageComponent from data!");
            return false;
        }

        return true;
    }
}