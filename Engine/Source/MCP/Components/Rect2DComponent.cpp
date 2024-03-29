// Rect2DComponent.cpp

#include "Rect2DComponent.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Components/TransformComponent.h"
#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    Rect2DComponent::Rect2DComponent(const float width, const float height, const RenderLayer layer, const int zOrder)
        : PrimitiveComponent(layer, zOrder)
        , m_width(width)
        , m_height(height)
    {
        //
    }

    void Rect2DComponent::Render() const
    {
        const Vec2 location = m_pTransformComponent->GetPosition();

        const float renderXPos = location.x - m_width / 2.f;
        const float renderYPos = location.y - m_height / 2.f;

        const RectF rect(renderXPos, renderYPos, m_width, m_height);

        if (m_renderType == RenderType::kFill)
        {
            DrawFillRect(rect, m_color);
        }

        else
        {
            DrawRect(rect, m_color);
        }
    }

    Rect2DComponent* Rect2DComponent::AddFromData(const XMLElement component)
    {
        // Width and Height
        const auto width = component.GetAttributeValue<float>("width");
        const auto height = component.GetAttributeValue<float>("height");

        // Get the Renderable info.
        const XMLElement renderableElement = component.GetChildElement("Renderable");
        if (!renderableElement.IsValid())
        {
            MCP_ERROR("Rect2DComponent", "Failed to add ImageComponent from Data! Couldn't find RenderType Element!");
            return nullptr;
        }

        // IRenderable Data.
        const auto layer =static_cast<RenderLayer>(renderableElement.GetAttributeValue<int>("layer"));
        const int zOrder = renderableElement.GetAttributeValue<int>("zOrder");

        // Add the component
        auto* pRect2DComponent = BLEACH_NEW(Rect2DComponent(width, height, layer, zOrder));
        if (!pRect2DComponent)
        {
            MCP_ERROR("Rect2DComponent", "Failed to add Rect2DComponent from data!");
            BLEACH_DELETE(pRect2DComponent);
            return nullptr;
        }

        // RenderType
        const XMLElement renderTypeElement = component.GetChildElement("RenderType");
        if (!renderTypeElement.IsValid())
        {
            MCP_ERROR("Rect2DComponent", "Failed to add ImageComponent from Data! Couldn't find RenderType Element!");
            BLEACH_DELETE(pRect2DComponent);
            return nullptr;
        }
        
        const char* pType = renderTypeElement.GetAttributeValue<const char*>("type");
        const RenderType type = pType == std::string("Fill") ? RenderType::kFill : RenderType::kOutline;
        pRect2DComponent->SetRenderType(type);

        // Color
        const XMLElement colorElement = renderTypeElement.GetSiblingElement("Color");
        if (!colorElement.IsValid())
        {
            MCP_ERROR("Rect2DComponent", "Failed to add ImageComponent from Data! Couldn't find Color Attribute!");
            BLEACH_DELETE(pRect2DComponent);
            return nullptr;
        }

        const auto r = colorElement.GetAttributeValue<uint8_t>("r");
        const auto g = colorElement.GetAttributeValue<uint8_t>("g");
        const auto b = colorElement.GetAttributeValue<uint8_t>("b");
        const auto alpha = colorElement.GetAttributeValue<uint8_t>("alpha", 255);
        pRect2DComponent->SetColor(r,g,b,alpha);

        return pRect2DComponent;
    }
}