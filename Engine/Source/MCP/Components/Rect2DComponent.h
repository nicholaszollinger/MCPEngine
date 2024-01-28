#pragma once
// Rect2DComponent.h

#include "PrimitiveComponent.h"

namespace mcp
{
    class Rect2DComponent final : public PrimitiveComponent
    {
        MCP_DEFINE_COMPONENT_ID(Rect2DComponent)
    
    private:
        float m_width;
        float m_height;

    public:
        Rect2DComponent(const float width, const float height, const RenderLayer = RenderLayer::kObject, const int zOrder = 0);

        virtual void Render() const override;

        void SetWidth(const float width) { m_width = width; }
        void SetHeight(const float height) { m_height = height; }
        void SetDimensions(const float width, const float height) { m_width = width; m_height = height; }
        
        static Rect2DComponent* AddFromData(const XMLElement element);
    };
}
