#pragma once
// PrimitiveComponent.h
// This is going to be the base class for primitive shapes like Rects, Circles.
// This is for rendering, not collisions.

#include "Component.h"
#include "../Scene/IRenderable.h"
#include "Utility/Types/Color.h"

namespace mcp
{
    class TransformComponent;

    class PrimitiveComponent : public Component, public IRenderable
    {
        MCP_DEFINE_COMPONENT_ID(PrimitiveComponent)

    public:
        enum class RenderType
        {
            kFill,
            kOutline,
        };

    protected:
        static constexpr Color kDefaultColor {0,255,100,255};

        TransformComponent* m_pTransformComponent;
        Color m_color;
        RenderType m_renderType;

    public:
        PrimitiveComponent(Object* pObject, const RenderLayer layer, const int zOrder);
        virtual ~PrimitiveComponent() override;

        virtual bool Init() override;
        virtual void Render() const override = 0;

        void SetColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t alpha = 255) { m_color = {r,g,b,alpha}; }
        void SetColor(const Color color) { m_color = color; }
        void SetRenderType(const RenderType type) { m_renderType = type;}
        virtual void SetIsActive(const bool isActive) override;

        static bool AddFromData(const XMLElement, Object*) { return false; }
    };
    
}
