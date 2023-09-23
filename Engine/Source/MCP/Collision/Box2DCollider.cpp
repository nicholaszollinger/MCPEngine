// Box2DCollider.cpp

#include "Box2DCollider.h"

#include "MCP/Debug/Log.h"
#include "MCP/Components/ColliderComponent.h"

namespace mcp
{
    Box2DCollider::Box2DCollider(const char* name, const bool isEnabled, const Vec2 position, const float width, const float height)
        : Collider(name, isEnabled, position)
        , m_width(width)
        , m_height(height)
    {
        //Log("Box: (%,%) w: % | h: %", position.x, position.y, width, height);
    }

    RectF Box2DCollider::GetEstimateRectRelative() const
    {
        // Place our point at the top left of the rect.
        Vec2 relativePos = m_relativePosition;
        relativePos.x -= m_width / 2.f;
        relativePos.y -= m_height / 2.f;

        return {relativePos.x, relativePos.y, m_width, m_height};
    }

    RectF Box2DCollider::GetEstimateRectWorld() const
    {
        Vec2 worldPos = m_relativePosition + m_pOwner->GetTransformComponent()->GetLocation();
        worldPos.x -= m_width / 2.f;
        worldPos.y -= m_height / 2.f;

        return {worldPos.x, worldPos.y, m_width, m_height};
    }
    
    bool Box2DCollider::AddFromData(const XMLElement colliderData, ColliderComponent* pComponent)
    {
        // Name
        const char* name = colliderData.GetAttribute<const char*>("name");

        // CollisionEnabled
        const bool isEnabled = colliderData.GetAttribute<bool>("collisionEnabled");

        // Position, Relative to the Transform component.
        Vec2 position;
        position.x = colliderData.GetAttribute<float>("x");
        position.y = colliderData.GetAttribute<float>("y");

        // Dimensions
        const auto width = colliderData.GetAttribute<float>("width");
        const auto height = colliderData.GetAttribute<float>("height");

        // Create the Collider
        auto* pCollider = BLEACH_NEW(Box2DCollider(name, isEnabled, position, width, height));

        // Add the collider to the Component.
        pComponent->AddCollider(pCollider);

        return true;
    }

#if RENDER_COLLIDER_VISUALS
    void Box2DCollider::Render() const
    {
        DrawRect(GetEstimateRectWorld(), kColliderDebugColor);
    }
#endif

}