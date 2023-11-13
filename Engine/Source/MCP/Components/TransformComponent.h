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
        TransformComponent* m_pParentTransform;
        Vec2 m_position;
        Vec2 m_scale;

    public:
        TransformComponent();
        TransformComponent(const Vec2 position);
        TransformComponent(const float xPos, const float yPos);
        TransformComponent(const Vec2 position, const Vec2 scale);

        // Position
        void SetPosition(const Vec2 position);
        // TODO: SetPosition vs SetLocalLocation()
        void AddToPosition(const Vec2 deltaPosition);
        void AddToPosition(const float deltaX, const float deltaY);
        void AddToPositionNoUpdate(const Vec2 deltaPosition);
        [[nodiscard]] Vec2 GetPosition() const;
        [[nodiscard]] Vec2 GetLocalPosition() const { return m_position; }

        // Scale
        void SetScale(const float xAxis, const float yAxis);
        void SetScale(const Vec2 scale);

        [[nodiscard]] Vec2 GetScale() const;
        [[nodiscard]] Vec2 GetLocalScale() const { return m_scale; }
        
        static TransformComponent* AddFromData(const XMLElement element);

    protected:
        virtual void OnOwnerParentSet(Object* pParent) override;
    };
}