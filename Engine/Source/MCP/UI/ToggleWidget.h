#pragma once
// ToggleWidget.h

#include "ButtonWidget.h"
#include "MCP/Core/Event/MulticastDelegate.h"

namespace mcp
{
    class ToggleWidget final : public ButtonWidget
    {
        MCP_DEFINE_WIDGET(ToggleWidget)

    public:
        using ValueChangedDelegate = MulticastDelegate<ToggleWidget*, bool>;
        ValueChangedDelegate m_onValueChanged;

    private:
        bool m_value;

    public:
        ToggleWidget(const WidgetConstructionData& data, const bool startVal, ButtonBehavior&& behavior);

        virtual bool PostLoadInit() override;
        void SetValue(const bool value);
        [[nodiscard]] bool GetValue() const { return m_value; }

        static ToggleWidget* AddFromData(const XMLElement element);
        static void RegisterLuaFunctions(lua_State* pState);

    private:
        virtual void OnExecute([[maybe_unused]] const Vec2 relativeClickPosition) override;
    };
}