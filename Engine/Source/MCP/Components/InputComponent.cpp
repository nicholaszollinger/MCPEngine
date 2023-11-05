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

    void InputComponent::BindAction(const InputAction* action, InputTriggerEvent event, void* pOwner, const InputActionCallback& memberFunction)
    {
        //
    }

    bool InputComponent::HasActionForControl(const MCPKey key)
    {
        return false;
    }

    void InputComponent::AddAction(InputAction* action)
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

    InputComponent* InputComponent::AddFromData(const XMLElement element)
    {
        // TODO
        return BLEACH_NEW(InputComponent);
    }
}
