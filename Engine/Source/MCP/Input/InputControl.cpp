// InputControl.cpp

#include "InputControl.h"

namespace mcp
{
    constexpr InputControl::InputControl(const ControlType controlType)
        : m_type(controlType)
        , m_rawValueType(InputValueType::kNone)
    {
        // Set the raw type based on the Control type.
        switch(m_type)
        {
            case ControlType::ModifierKey: break;

            case ControlType::GamepadButton:
            case ControlType::Key:
            case ControlType::MouseButton:
            {
                m_rawValueType = InputValueType::kBoolean;
                break;
            }

            case ControlType::Axis1D:
            {
                m_rawValueType = InputValueType::kAxis1D;
                break;
            }

            case ControlType::Axis2D:
            {
                m_rawValueType = InputValueType::kAxis2D;
            }
        }
    }
}