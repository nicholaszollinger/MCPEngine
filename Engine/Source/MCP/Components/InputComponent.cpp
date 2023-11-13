// InputComponent.cpp

#include "InputComponent.h"
#include "MCP/Scene/Object.h"
#include "MCP/Scene/WorldLayer.h"

namespace mcp
{
    InputComponent::InputComponent()
        : Component(true)
    {
        //
    }

    void InputComponent::BindAction([[maybe_unused]] const InputAction* action, [[maybe_unused]] InputTriggerEvent event, [[maybe_unused]] void* pOwner, [[maybe_unused]] const InputActionCallback& memberFunction)
    {
        //
    }

    bool InputComponent::HasActionForControl([[maybe_unused]] const MCPKey key)
    {
        return false;
    }

    void InputComponent::AddAction([[maybe_unused]] InputAction* action)
    {
        //
    }

    void InputComponent::OnActive()
    {
        // Listen to input
        GetOwner()->GetWorld()->AddInputListener(this);
    }

    void InputComponent::OnInactive()
    {
        // Stop listening to input
        GetOwner()->GetWorld()->RemoveInputListener(this);
    }

    void InputComponent::TickInput([[maybe_unused]] const float deltaTimeMs)
    {
        // I need to update
    }

    InputComponent* InputComponent::AddFromData([[maybe_unused]] const XMLElement element)
    {
        // TODO
        return BLEACH_NEW(InputComponent);
    }
}
