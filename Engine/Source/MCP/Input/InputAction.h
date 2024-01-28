#pragma once
// InputAction.h
// TODO: I didn't have time to complete the InputSystem the way I wanted.

#include "InputModifier.h"
#include "MCP/Core/Event/MulticastDelegate.h"

namespace mcp
{
    // Stages of the lifetime of an Input Event.
    enum class InputTriggerEvent
    {
        kNone,
        kTriggered,
        kStarted,
        kOnGoing,
        kCanceled,
        kCompleted,
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Struct that defines when an InputActionInstance is created, and what type of value that the InputAction creates.
    //-----------------------------------------------------------------------------------------------------------------------------
    struct InputAction
    {
        using InputActionDelegate = MulticastDelegate<const InputActionValue&>;

        std::vector<InputModifier> m_modifiers;             // An array of modifiers that will change the final value of the final InputActionValue.
        InputValueType m_type = InputValueType::kBoolean;   // Defines what value type we are interested in reading from an InputActionValue.

        bool m_consumeInput = true;                         // Whether this action will consume the input after it is read.
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : An instance of an InputAction that has occurred. This is created when an ApplicationEvent would start an InputAction
    //-----------------------------------------------------------------------------------------------------------------------------
    class InputActionInstance
    {
        InputAction* m_pAction = nullptr;
        InputTriggerEvent m_triggerEvent = InputTriggerEvent::kNone;    // The current trigger state of this Instance.
        InputActionValue m_value;                                       // The value of this instance.

    public:
        InputActionInstance() = default;
    };
}