// Rect2DComponent.cpp

#include "Rect2DComponent.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Components/TransformComponent.h"
#include "MCP/Scene/Object.h"

namespace mcp
{
    Rect2DComponent::Rect2DComponent(Object* pObject, const float width, const float height, const RenderLayer layer, const int zOrder)
        : PrimitiveComponent(pObject, layer, zOrder)
        , m_width(width)
        , m_height(height)
    {
        //
    }

    void Rect2DComponent::Render() const
    {
        const Vec2 location = m_pTransformComponent->GetLocation();

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

    bool Rect2DComponent::AddFromData(const XMLElement component, Object* pOwner)
    {
        // Width and Height
        const auto width = component.GetAttribute<float>("width");
        const auto height = component.GetAttribute<float>("height");

        // Get the Renderable info.
        const XMLElement renderableElement = component.GetChildElement("Renderable");
        if (!renderableElement.IsValid())
        {
            MCP_ERROR("Rect2DComponent", "Failed to add ImageComponent from Data! Couldn't find RenderType Element!");
            return false;
        }

        const auto layer =static_cast<RenderLayer>(renderableElement.GetAttribute<int>("layer"));
        const int zOrder = renderableElement.GetAttribute<int>("zOrder");

        // Add the component
        auto* pRect2DComponent = pOwner->AddComponent<Rect2DComponent>(width, height, layer, zOrder);
        if (!pRect2DComponent)
        {
            MCP_ERROR("Rect2DComponent", "Failed to add Rect2DComponent from data!");
            return false;
        }

        // RenderType
        const XMLElement renderTypeElement = component.GetChildElement("RenderType");
        if (!renderTypeElement.IsValid())
        {
            MCP_ERROR("Rect2DComponent", "Failed to add ImageComponent from Data! Couldn't find RenderType Element!");
            return false;
        }
        
        const char* pType = renderTypeElement.GetAttribute<const char*>("type");
        const RenderType type = pType == std::string("Fill") ? RenderType::kFill : RenderType::kOutline;
        pRect2DComponent->SetRenderType(type);

        // Color
        const XMLElement colorElement = renderTypeElement.GetSiblingElement("Color");
        if (!colorElement.IsValid())
        {
            MCP_ERROR("Rect2DComponent", "Failed to add ImageComponent from Data! Couldn't find Color Attribute!");
            return false;
        }

        const auto r = colorElement.GetAttribute<uint8_t>("r");
        const auto g = colorElement.GetAttribute<uint8_t>("g");
        const auto b = colorElement.GetAttribute<uint8_t>("b");
        const auto alpha = colorElement.GetAttribute<uint8_t>("alpha", 255);
        pRect2DComponent->SetColor(r,g,b,alpha);

        return true;
    }

}