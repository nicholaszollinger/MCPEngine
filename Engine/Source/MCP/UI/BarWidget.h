#pragma once
// BarWidget.h

#include "Widget.h"
#include "MCP/Core/Event/MulticastDelegate.h"

namespace mcp
{
    class BarWidget final : public Widget
    {
        MCP_DEFINE_WIDGET(BarWidget)

    public:
        using ValueChangedDelegate = MulticastDelegate<int>;
        ValueChangedDelegate m_onValueChanged;

    private:
        Script m_onValueChangedScript;
        int m_min;
        int m_max;
        int m_step; // TODO: Set this up.
        int m_value;

    public:
        BarWidget(const WidgetConstructionData& data, const int min, const int max, const int step, const int startVal, LuaResourcePtr&& onValueChangedBehavior);
        virtual bool PostLoadInit() override;

        void SetValue(int value);
        [[nodiscard]] float GetValueNormalized() const;
        [[nodiscard]] int GetValue() const { return m_value; }
        [[nodiscard]] int GetMax() const { return m_max; }

        static BarWidget* AddFromData(const XMLElement element);
        static void RegisterLuaFunctions(lua_State* pState);
    };
}
