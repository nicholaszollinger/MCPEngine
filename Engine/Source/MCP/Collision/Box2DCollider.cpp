// Box2DCollider.cpp

#include "Box2DCollider.h"

#include "MCP/Debug/Log.h"
#include "MCP/Components/ColliderComponent.h"

#ifdef MCP_DATA_PARSER_TINYXML2
    #include "Platform/TinyXML2/tinyxml2.h"
#else
    #error "ImageComponent does not have support a the currently defined Data Parser!"
#endif

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

#ifdef MCP_DATA_PARSER_TINYXML2
    bool Box2DCollider::AddFromData(const void* pFileData, ColliderComponent* pComponent)
    {
        auto* pBox2DElement = static_cast<const tinyxml2::XMLElement*>(pFileData);

        // Name
        const char* name = pBox2DElement->Attribute("name");

        // CollisionEnabled
        const bool isEnabled = pBox2DElement->BoolAttribute("collisionEnabled");

        // Position, Relative to the Transform component.
        Vec2 position;
        position.x = pBox2DElement->FloatAttribute("x");
        position.y = pBox2DElement->FloatAttribute("y");

        // Dimensions
        const float width = pBox2DElement->FloatAttribute("width");
        const float height = pBox2DElement->FloatAttribute("height");

        // Create the Collider
        auto* pCollider = BLEACH_NEW(Box2DCollider(name, isEnabled, position, width, height));

        // Add the collider to the Component.
        pComponent->AddCollider(pCollider);

        return true;
    }

#else
    bool Box2DCollider::AddNewFromData(const void*, ColliderComponent*)
    {
        LogError("Failed to add Box2DCollider from data! No implementation for the current parser!");
        return false;
    }
#endif

#if RENDER_COLLIDER_VISUALS
    void Box2DCollider::Render() const
    {
        DrawRect(GetEstimateRectWorld(), kColliderDebugColor);
    }
#endif

}