// Rect2DComponent.cpp

#include "Rect2DComponent.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Components/TransformComponent.h"
#include "MCP/Scene/Object.h"

#ifdef MCP_DATA_PARSER_TINYXML2
    #include "Platform/TinyXML2/tinyxml2.h"
#else
    #error "Rect2DComponent does not have support a the currently defined Data Parser!"
#endif

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

    bool Rect2DComponent::AddFromData(const void* pFileData, Object* pOwner)
    {
        auto* pRect2DElement = static_cast<const tinyxml2::XMLElement*>(pFileData);

        // Width and Height
        const float width = pRect2DElement->FloatAttribute("width");
        const float height = pRect2DElement->FloatAttribute("height");

        // Add the component
        auto* pRect2DComponent = pOwner->AddComponent<Rect2DComponent>(width, height);
        if (!pRect2DComponent)
        {
            LogError("Failed to add Rect2DComponent from data!");
            return false;
        }

        // RenderType
        const auto* pRenderTypeElement = pRect2DElement->FirstChildElement("RenderType");
        if (!pRenderTypeElement)
        {
            LogError("Failed to add ImageComponent from Data! Couldn't find RenderType Attribute!");
            return false;
        }

        const char* pType = pRenderTypeElement->Attribute("type");
        const RenderType type = pType == std::string("Fill") ? RenderType::kFill : RenderType::kOutline;
        pRect2DComponent->SetRenderType(type);

        // Color
        const auto* pColorElement = pRenderTypeElement->NextSiblingElement("Color");
        if (!pColorElement)
        {
            LogError("Failed to add ImageComponent from Data! Couldn't find Color Attribute!");
            return false;
        }

        const auto r = static_cast<uint8_t>(pColorElement->IntAttribute("r"));
        const auto g = static_cast<uint8_t>(pColorElement->IntAttribute("g"));
        const auto b = static_cast<uint8_t>(pColorElement->IntAttribute("b"));
        const auto alpha = static_cast<uint8_t>(pColorElement->IntAttribute("alpha"));
        pRect2DComponent->SetColor(r,g,b,alpha);

        return true;
    }

}