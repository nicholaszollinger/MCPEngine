#pragma once
#include <vector>

#include "InputCodes.h"

namespace mcp
{
    enum class InputValueType
    {
        kNone,
        kBoolean,   // Value returns either true or false. Good for Buttons
        kAxis1D,    // Value returns a float from [0, 1]. Good for triggers.
        kAxis2D     // Value returns a normalized Vec2. Good for XY movement including mouse movement, joysticks, or arrow keys.
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Unified Enum for all possible input controls.
    //-----------------------------------------------------------------------------------------------------------------------------
    class InputControl
    {
    public:
        enum class ControlType
        {
            GamepadButton,  // Button on a Gamepad: "A"
            MouseButton,    // Mouse Button: "LeftClick"
            Key,            // Keyboard control: "W"
            ModifierKey,    // Keyboard modifier: "Shift"
            Axis1D,         // "Trigger" on a controller
            Axis2D,         // "MouseMovement" or a "Joystick"
        };

    private:

        // This is the value type that is generated for this control when being read from input. For example, a Key on the keyboard
        // can only produce a 0 or a 1. The mouse movement produces a 2D axis.
        ControlType m_type;
        InputValueType m_rawValueType;

    public:
        constexpr InputControl(const ControlType controlType);

        [[nodiscard]] ControlType GetControlType() const { return m_type; }
        [[nodiscard]] InputValueType GetValueType() const { return m_rawValueType; }
    };

    class InputControlManager
    {

    public:
        bool Initialize();

        InputValueType GetValueTypeForControl(const MCPKey key) const;
    };
}
