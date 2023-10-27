// TransformComponent.cpp

#include "TransformComponent.h"

#include "MCP/Scene/Object.h"

namespace mcp
{
    TransformComponent::TransformComponent()
        : Component(true)
        , m_position{}
    {
        //
    }

    TransformComponent::TransformComponent(const Vec2& position)
        : Component(true)
        , m_position(position)
    {
        //
    }

    TransformComponent::TransformComponent(const float xPos, const float yPos)
        : Component(true)
        , m_position(xPos, yPos)
    {
        //
    }

    void TransformComponent::AddToLocation(const float deltaX, const float deltaY)
    {
        m_position.x += deltaX;
        m_position.y += deltaY;

        m_onLocationUpdated.Broadcast(m_position);
    }

    void TransformComponent::AddToLocation(const Vec2 deltaPosition)
    {
        m_position += deltaPosition;
        m_onLocationUpdated.Broadcast(m_position);
    }

    void TransformComponent::AddToLocationNoUpdate(const Vec2 deltaPosition)
    {
        m_position += deltaPosition;
    }

    void TransformComponent::SetLocation(const Vec2 position)
    {
        m_position = position;
        m_onLocationUpdated.Broadcast(m_position);
    }

    TransformComponent* TransformComponent::AddFromData(const XMLElement element)
    {
        const auto x = element.GetAttributeValue<float>("x", 0.f);
        const auto y = element.GetAttributeValue<float>("y", 0.f);

        return BLEACH_NEW(TransformComponent(x, y));
    }

}