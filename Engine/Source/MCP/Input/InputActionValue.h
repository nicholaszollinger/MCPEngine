#pragma once
// InputActionValue.h

#include "InputControl.h"
#include "Utility/Types/Vector2.h"

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Class that stores the Value from an input action. For example: If you hit the 'Space' key, this could store
    ///         'true'. Or if you are pushing a key to move right, this could store the value '[1, 0]'.
    //-----------------------------------------------------------------------------------------------------------------------------
    class InputActionValue
    {
        Vec2 m_value = Vec2::ZeroVector();
        InputValueType m_type = InputValueType::kBoolean;

    public:
        InputActionValue() = default;
        InputActionValue(const bool val);
        InputActionValue(const float val);
        InputActionValue(const Vec2 val);
        InputActionValue(const Vec2 val, const InputValueType type);

        template<typename ReturnType>
        ReturnType Get() const;

        InputActionValue& operator+=(const InputActionValue& right)
        {
            m_value += right.m_value;

            // Promote the type to the highest value.
            m_type = std::max(m_type, right.m_type);
            return *this;
        }

        InputActionValue operator+(const InputActionValue& right) const
        {
            InputActionValue result = *this;
            result += right;
            return result;
        }

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Get the type of Value that this holds.
        //-----------------------------------------------------------------------------------------------------------------------------
        [[nodiscard]] InputValueType GetType() const { return m_type; }
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Returns the value as a boolean.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <>
    inline bool InputActionValue::Get() const
    {
        return m_value.x > 0.f;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Returns the value as a float.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <>
    inline float InputActionValue::Get() const
    {
        return m_value.x;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Return the value as a Vector2.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <>
    inline Vec2 InputActionValue::Get() const
    {
        return m_value;
    }
}