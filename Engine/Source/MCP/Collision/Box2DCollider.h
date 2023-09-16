#pragma once
// Box2DCollider

#include "Collider.h"

namespace mcp
{
    class Box2DCollider final : public Collider
    {
        MCP_DEFINE_COLLIDER_TYPE_ID(Box2DCollider)

        float m_width;
        float m_height;
        
    public:
        Box2DCollider(const char* name, const bool isEnabled, const Vec2 position, const float width, const float height);
        
        [[nodiscard]] virtual RectF GetEstimateRectRelative() const override;
        [[nodiscard]] virtual RectF GetEstimateRectWorld() const override;
        void SetWidth(const float width) { m_width = width; }
        void SetHeight(const float height) { m_width = height; }

        static bool AddFromData(const void* pFileData, class ColliderComponent* pComponent);

        //--------------------------------------------------------------------
        //  Debug Rendering
        //--------------------------------------------------------------------
#if RENDER_COLLIDER_VISUALS
        virtual void Render() const override final;
#endif
    };
}