// ImageWidget.cpp

#include "ImageWidget.h"

#include "MCP/Graphics/Graphics.h"
#include "MCP/Scene/UILayer.h"

namespace mcp
{
    ImageWidget::ImageWidget(const WidgetConstructionData& data, const char* pImageFilePath)
        : Widget(data)
        , IRenderable(RenderLayer::kDebugOverlay, 0)
        , m_crop{}
    {
        m_texture.Load(pImageFilePath);
        const auto imageSize = m_texture.GetBaseSize();
        m_crop = RectInt{0,0, imageSize.x, imageSize.y};
    }

    ImageWidget::~ImageWidget()
    {
        m_pUILayer->RemoveRenderable(this);
    }

    bool ImageWidget::Init()
    {
        m_pUILayer->AddRenderable(this);
        return true;
    }

    void ImageWidget::Render() const
    {
        auto rect = GetScreenRect();
        rect.x -= rect.width / 2.f;
        rect.y -= rect.height / 2.f;

        TextureRenderData renderData;
        renderData.pTexture = m_texture.Get();
        renderData.angle = m_renderAngle;
        renderData.crop = m_crop;
        renderData.destinationRect = rect;
        renderData.flip = m_flip;

        DrawTexture(renderData);
    }

    void ImageWidget::SetActive(const bool isActive)
    {
        if (isActive != m_isActive)
        {
            if (isActive)
            {
                m_pUILayer->AddRenderable(this);
            }

            else
            {
                m_pUILayer->RemoveRenderable(this);
            }

            m_isActive = isActive;
        }
    }

    void ImageWidget::OnParentActiveChanged(const bool parentActiveState)
    {
        // If we are being set active, add the renderable
        if (parentActiveState)
            m_pUILayer->AddRenderable(this);

        // And vice versa.
        else
            m_pUILayer->RemoveRenderable(this);
    }

    ImageWidget* ImageWidget::AddFromData(const XMLElement element)
    {
        const char* pFilepath = element.GetAttribute<const char*>("path");

        RectF rect;
        rect.x = element.GetAttribute<float>("x");
        rect.y = element.GetAttribute<float>("y");
        rect.width = element.GetAttribute<float>("width");
        rect.height = element.GetAttribute<float>("height");

        WidgetConstructionData data;
        data.rect = rect;
        data.anchor.x = element.GetAttribute<float>("anchorX", 0.5f);
        data.anchor.y = element.GetAttribute<float>("anchorY", 0.5f);
        data.isInteractable = false;
        data.zOffset = element.GetAttribute<int>("zOffset", 1);

        return BLEACH_NEW(ImageWidget(data, pFilepath));
    }
}
