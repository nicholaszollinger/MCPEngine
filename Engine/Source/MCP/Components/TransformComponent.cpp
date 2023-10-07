// TransformComponent.cpp

#include "TransformComponent.h"

#include "MCP/Scene/Object.h"

namespace mcp
{
    TransformComponent::TransformComponent(Object* pOwner, const Vec2& position)
        : Component(pOwner)
        , m_position(position)
    {
        //
    }

    TransformComponent::TransformComponent(Object* pOwner, const float xPos, const float yPos)
        : Component(pOwner)
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
    
    bool TransformComponent::AddFromData(const XMLElement component, Object* pOwner)
    {
        const auto x = component.GetAttributeValue<float>("x");
        const auto y = component.GetAttributeValue<float>("y");

        if (!pOwner->AddComponent<TransformComponent>(x, y))
        {
            MCP_ERROR("TransformComponent", "Failed to add TransformComponent from data!");
            return false;
        }

        return true;
    }
}