// TransformComponent.cpp

#include "TransformComponent.h"

#include "MCP/Scene/Object.h"

namespace mcp
{
    TransformComponent::TransformComponent()
        : Component(true)
        , m_pParentTransform(nullptr)
        , m_position{}
    {
        //
    }

    TransformComponent::TransformComponent(const Vec2 position)
        : Component(true)
        , m_pParentTransform(nullptr)
        , m_position(position)
        , m_scale(1.f, 1.f)
    {
        //
    }

    TransformComponent::TransformComponent(const float xPos, const float yPos)
        : Component(true)
        , m_pParentTransform(nullptr)
        , m_position(xPos, yPos)
        , m_scale(1.f, 1.f)
    {
        //
    }

    TransformComponent::TransformComponent(const Vec2 position, const Vec2 scale)
        : Component(true)
        , m_pParentTransform(nullptr)
        , m_position(position)
        , m_scale(scale)
    {
        //
    }

    void TransformComponent::AddToPosition(const float deltaX, const float deltaY)
    {
        m_position.x += deltaX;
        m_position.y += deltaY;

        m_onLocationUpdated.Broadcast(m_position);
    }

    void TransformComponent::AddToPosition(const Vec2 deltaPosition)
    {
        m_position += deltaPosition;
        m_onLocationUpdated.Broadcast(m_position);
    }

    void TransformComponent::AddToPositionNoUpdate(const Vec2 deltaPosition)
    {
        m_position += deltaPosition;
    }

    void TransformComponent::SetPosition(const Vec2 position)
    {
        m_position = position;
        m_onLocationUpdated.Broadcast(m_position);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Returns the World position of the Transform.
    //-----------------------------------------------------------------------------------------------------------------------------
    Vec2 TransformComponent::GetPosition() const
    {
        if (m_pParentTransform)
        {
            return m_pParentTransform->GetPosition() + m_position;
        }

        return m_position;
    }

    void TransformComponent::SetScale(const Vec2 scale)
    {
        m_scale = scale;
    }

    void TransformComponent::SetScale(const float xAxis, const float yAxis)
    {
        m_scale.x = xAxis;
        m_scale.y = yAxis;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the Scale of this Transform. The final scale value is dependent on the parent transforms. If you want the
    ///         local scale of this transform, use GetLocalScale();
    //-----------------------------------------------------------------------------------------------------------------------------
    Vec2 TransformComponent::GetScale() const
    {
        if (m_pParentTransform)
        {
            return m_scale * m_pParentTransform->GetScale();
        }

        return m_scale;
    }

    void TransformComponent::OnOwnerParentSet(Object* pParent)
    {
        // If our parent is now null and we had a parent transform,
        if (!pParent && m_pParentTransform)
        {
            m_pParentTransform = nullptr;
            return;
        }

        // If our parent has a transform:
        if (auto* pTransform = pParent->GetComponent<TransformComponent>())
        {
            m_pParentTransform = pTransform;
        }
    }

    TransformComponent* TransformComponent::AddFromData(const XMLElement element)
    {
        // Position
        Vec2 position;
        position.x = element.GetAttributeValue<float>("x", 0.f);
        position.y = element.GetAttributeValue<float>("y", 0.f);

        // Scale
        Vec2 scale(1.f, 1.f);
        const auto scaleElement = element.GetChildElement("Scale");
        if (scaleElement.IsValid())
        {
            scale.x = scaleElement.GetAttributeValue<float>("x", 1.f);
            scale.y = scaleElement.GetAttributeValue<float>("y", 1.f);
        }

        return BLEACH_NEW(TransformComponent(position, scale));
    }

}