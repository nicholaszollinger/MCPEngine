// InputModifier.cpp

#include "InputModifier.h"

namespace mcp
{
    InputActionValue Negate::ModifyInputValue(const InputActionValue currentValue) const
    {
        auto val = currentValue.Get<Vec2>();
        val = -val;

        return InputActionValue(val, currentValue.GetType());
    }

    InputActionValue SwapAxes::ModifyInputValue(const InputActionValue currentValue) const
    {
        auto val = currentValue.Get<Vec2>();
        val.SwapAxes();
        return InputActionValue(val, currentValue.GetType());
    }
}