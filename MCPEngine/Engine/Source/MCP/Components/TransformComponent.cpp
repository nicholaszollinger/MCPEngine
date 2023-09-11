// TransformComponent.cpp

#include "TransformComponent.h"

#include "MCP/Application/Core/Config.h"
#include "MCP/Objects/Object.h"

#ifdef MCP_DATA_PARSER_TINYXML2
    #include "Platform/TinyXML2/tinyxml2.h"
#else
    #error "TransformComponent does not have support a the currently defined Data Parser!"
#endif

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

    void TransformComponent::SetLocation(const Vec2 position)
    {
        m_position = position;
        m_onLocationUpdated.Broadcast(m_position);
    }

#ifdef MCP_DATA_PARSER_TINYXML2
    bool TransformComponent::AddFromData(const void* pFileData, Object* pOwner)
    {
        const auto* pTransformElement = static_cast<const tinyxml2::XMLElement*>(pFileData);

        const float x = pTransformElement->FloatAttribute("x");
        const float y = pTransformElement->FloatAttribute("y");

        if (!pOwner->AddComponent<TransformComponent>(x, y))
        {
            LogError("Failed to add TransformComponent from data!");
            return false;
        }

        return true;
    }
#endif
}