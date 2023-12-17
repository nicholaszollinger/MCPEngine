#pragma once
// TransformComponent.h

#include "Component.h"
#include "Utility/Types/Vector2.h"

namespace mcp
{
    class TransformComponent final : public Component
    {
        MCP_DEFINE_COMPONENT_ID(TransformComponent)

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
        // TODO: SetPosition vs SetLocalPosition()
        void AddToPosition(const Vec2 deltaPosition);
        void AddToPosition(const float deltaX, const float deltaY);
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