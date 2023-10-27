#pragma once
// TransformComponent.h

#include "Component.h"
#include "MCP/Core/Event/MulticastDelegate.h"
#include "MCP/Core/Event/Message.h"
#include "Utility/Types/Vector2.h"

namespace mcp
{
    using OnLocationUpdated = MulticastDelegate<const Vec2>;
    //using OnRotationUpdated = MulticastDelegate<>; // To add later.

    class TransformComponent final : public Component
    {
        MCP_DEFINE_COMPONENT_ID(TransformComponent)

    public:
        OnLocationUpdated m_onLocationUpdated;

    private:
        Vec2 m_position;

    public:
        TransformComponent();
        TransformComponent(const Vec2& position);
        TransformComponent(const float xPos, const float yPos);

        void SetLocation(const Vec2 position);
        void AddToLocation(const Vec2 deltaPosition);
        void AddToLocation(const float deltaX, const float deltaY);
        void AddToLocationNoUpdate(const Vec2 deltaPosition);
        [[nodiscard]] Vec2 GetLocation() const { return m_position; }
        
        static TransformComponent* AddFromData(const XMLElement element);
    };
}