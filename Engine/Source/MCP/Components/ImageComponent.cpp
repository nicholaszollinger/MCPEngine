// ImageComponent.h

#include "ImageComponent.h"

#include "TransformComponent.h"
#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"
#include "MCP/Graphics/Graphics.h"

namespace mcp
{
    ImageComponent::ImageComponent(const RenderLayer layer, const int zOrder)
        : Component(true)
        , IRenderable(layer, zOrder)
        , m_pTransformComponent(nullptr)
        , m_crop()
        , m_tint(Color::White())
        , m_renderAngle(0.0)
        , m_flip(RenderFlip2D::kNone)
        , m_sizedToContent(true)
    {
        //
    }

    ImageComponent::ImageComponent(const char* pTextureFilepath, const RectInt& crop, const Vec2 scale, const Color color, const RenderLayer layer, const int zOrder)
        : Component(true)
        , IRenderable(layer, zOrder)
        , m_pTransformComponent(nullptr)
        , m_crop(crop)
        , m_scale(scale)
        , m_tint(color)
        , m_renderAngle(0.0)
        , m_flip(RenderFlip2D::kNone)
        , m_sizedToContent(true)
    {
        if (!pTextureFilepath)
        {
            return;
        }

        if (!m_texture.Load({pTextureFilepath, nullptr, false}))
        {
            MCP_WARN("ImageComponent", "Failed to load texture!");
        }
    }

    ImageComponent::ImageComponent(const ImageComponentConstructionData& data)
        : Component(data.componentData)
        , IRenderable(RenderLayer::kWorld, data.zOrder)
        , m_pTransformComponent(nullptr)
        , m_crop(data.crop)
        , m_renderAngle(0.0)
        , m_flip(RenderFlip2D::kNone)
        , m_sizedToContent(data.sizedToContent)
    {
        // If we don't have an image path, then just return.
        if (!data.pImagePath)
        {
            return;
        }

        if (!m_texture.Load({data.pImagePath, nullptr, false}))
        {
            MCP_WARN("ImageComponent", "Failed to load texture! Path: ", data.pImagePath);
            return;
        }

        // If we are sized to our content, then we are going to set the crop to contain the whole image.
        if (m_sizedToContent)
        {
            const auto imageSize = m_texture.GetTextureSize();
            m_crop = RectInt{0,0, imageSize.x, imageSize.y};
        }
    }

    bool ImageComponent::Init()
    {
        m_pTransformComponent = GetOwner()->GetComponent<TransformComponent>();
        if (!m_pTransformComponent)
        {
            MCP_ERROR("ImageComponent", "Failed to initialize ImageComponent! TransformComponent was nullptr!");
            return false;
        }

        if (IsActive() && m_texture.IsValid())
        {
            GetOwner()->GetWorld()->AddRenderable(this);
        }

        return true;
    }

    void ImageComponent::OnOwnerParentSet(Object* pParent)
    {
        if (!pParent)
        {
            return;
        }

        // HACK: I need a way for a renderable component to know that their owner has a renderable component.
        if (auto* pImage = pParent->GetComponent<ImageComponent>())
        {
            SetRenderableParent(pImage);
        }

        else
        {
            SetRenderableParent(nullptr);
        }
    }

    void ImageComponent::OnActive()
    {
        if (m_texture.IsValid())
            GetOwner()->GetWorld()->AddRenderable(this);
    }

    void ImageComponent::OnInactive()
    {
        if (m_texture.IsValid())
            GetOwner()->GetWorld()->RemoveRenderable(this);
    }

    void ImageComponent::Render() const
    {
        MCP_CHECK_MSG(m_pTransformComponent, "Failed to Render ImageComponent! TransformComponent was nullptr!");

        const Vec2 scale = m_pTransformComponent->GetScale();
        const float width = scale.x * static_cast<float>(m_crop.width);
        const float height = scale.y * static_cast<float>(m_crop.height);

        const Vec2 location = m_pTransformComponent->GetPosition();
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

    void ImageComponent::SetTexture(const Texture* pTexture)
    {
        if (!pTexture || !pTexture->IsValid())
        {
            // If we were already invalid, return.
            if (!m_texture.IsValid())
                return;

            m_texture = kInvalidTexture;

            // If this is active, we need to not render the null texture.
            if (IsActive())
            {
                GetOwner()->GetWorld()->RemoveRenderable(this);
            }
        }

        else
        {
            // Get if our current texture is valid.
            const bool textureWasValid = m_texture.IsValid();

            // Assign the new texture:
            m_texture = *pTexture;

            // If we are sized to our content, adjust our crop.
            if (m_sizedToContent)
            {
                const auto imageSize = m_texture.GetTextureSize();
                m_crop = RectInt{0,0, imageSize.x, imageSize.y};
            }

            // If we are active and our texture was previously invalid (null texture), then add us to World for rendering.
            if (IsActive() && !textureWasValid)
            {
                GetOwner()->GetWorld()->AddRenderable(this);
            }
        }
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

    ImageComponent* ImageComponent::AddFromData(const XMLElement element)
    {
        ImageComponentConstructionData data;

        // Base Component data:
        data.componentData = GetComponentConstructionData(element);

        // File Path
        data.pImagePath = element.GetAttributeValue<const char*>("imagePath", nullptr);

        // ZOrder
        data.zOrder = element.GetAttributeValue<int>("zOrder", 0);

        // Size to content:
        data.sizedToContent = element.GetAttributeValue<bool>("sizedToContent", true);

        // Crop
        const XMLElement cropElement = element.GetChildElement("Crop");
        if (cropElement.IsValid())
        {
            /*MCP_ERROR("ImageComponent","Failed to add ImageComponent from Data! Couldn't find Crop Attribute!");
            return nullptr;*/

            data.crop.x = cropElement.GetAttributeValue<int>("x", 0);
            data.crop.y = cropElement.GetAttributeValue<int>("y", 0);
            data.crop.width = cropElement.GetAttributeValue<int>("w", 0);
            data.crop.height = cropElement.GetAttributeValue<int>("h", 0);
        }

        // If there is no crop element, then we know that we want to size to the content.
        else
        {
            data.sizedToContent = true;
        }
        
        const XMLElement tintElement = element.GetChildElement("Color");
        if (tintElement.IsValid())
        {
            data.tint.r = tintElement.GetAttributeValue<uint8_t>("r");
            data.tint.g = tintElement.GetAttributeValue<uint8_t>("g");
            data.tint.b = tintElement.GetAttributeValue<uint8_t>("b");
            data.tint.alpha = tintElement.GetAttributeValue<uint8_t>("alpha");
        }

        // TODO: Refactor when we get rid of the layer concept:
        // IRenderable Data.
        const XMLElement renderableElement = element.GetChildElement("Renderable");
        if (renderableElement.IsValid())
        {
            //const auto layer =static_cast<RenderLayer>(renderableElement.GetAttributeValue<int>("layer"));
            data.zOrder = renderableElement.GetAttributeValue<int>("zOrder");
        }

        return BLEACH_NEW(ImageComponent(data));
    }

}