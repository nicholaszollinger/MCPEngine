#pragma once
// InputListener.h
// TODO: I didn't have time to complete the InputSystem the way I wanted.

#include "MCP/Input/InputAction.h"

namespace mcp
{
    class InputActionBinding
    {
        const InputAction* m_pAction = nullptr;
        InputTriggerEvent m_trigger = InputTriggerEvent::kNone;

    public:
        InputActionBinding() = default;
        InputActionBinding(const InputAction* pAction, const InputTriggerEvent trigger) : m_pAction(pAction), m_trigger(trigger) {}

        [[nodiscard]] const InputAction* GetAction() const { return m_pAction; }
        [[nodiscard]] InputTriggerEvent GetTrigger() const { return m_trigger; }
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Defines an interface for a SceneEntity to Listen for input.
    //-----------------------------------------------------------------------------------------------------------------------------
    class InputListener
    {
        // ID?
    protected:
        std::vector<InputActionBinding> m_actionBindings;

    public:
        virtual ~InputListener() = default;
    };
}