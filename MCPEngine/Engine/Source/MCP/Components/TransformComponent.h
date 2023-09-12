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
        MCP_DEFINE_COMPONENT_DEFAULT_CONSTRUCTOR(TransformComponent)
        TransformComponent(Object* pOwner, const Vec2& position);
        TransformComponent(Object* pOwner, const float xPos, const float yPos);

        void SetLocation(const Vec2 position);
        void AddToLocation(const Vec2 deltaPosition);
        void AddToLocation(const float deltaX, const float deltaY);
        [[nodiscard]] Vec2 GetLocation() const { return m_position; }
            
    public:
        static bool AddFromData(const void* pFileData, Object* pOwner);

    };
}