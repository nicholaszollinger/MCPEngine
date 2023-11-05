#pragma once

#include "InputActionValue.h"

namespace mcp
{
    class InputModifier
    {
    public:
        virtual ~InputModifier() = default;

        virtual InputActionValue ModifyInputValue(const InputActionValue currentValue) const { return currentValue; }
    };

    class Negate final : public InputModifier
    {
    public:
        virtual InputActionValue ModifyInputValue(const InputActionValue currentValue) const override;
    };

    class SwapAxes final : public InputModifier
    {
    public:
        virtual InputActionValue ModifyInputValue(const InputActionValue currentValue) const override;
    };
}