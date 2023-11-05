// InputActionValue.cpp

#include "InputAction.h"

namespace mcp
{
    InputActionValue::InputActionValue(const bool val)
        : m_value(val? Vec2(1.f, 0.f) : Vec2::ZeroVector())
        , m_type(InputValueType::kBoolean)
    {
       // 
    }

    InputActionValue::InputActionValue(const float val)
        : m_value(val, 0.f)
        , m_type(InputValueType::kAxis1D)
    {
        //
    }

    InputActionValue::InputActionValue(const Vec2 val)
        : m_value(val)
        , m_type(InputValueType::kAxis2D)
    {
        //
    }

    InputActionValue::InputActionValue(const Vec2 val, const InputValueType type)
        : m_value(val)
        , m_type(type)
    {
        //
    }
}