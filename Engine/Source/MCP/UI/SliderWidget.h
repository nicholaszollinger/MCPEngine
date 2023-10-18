#pragma once
// SliderWidget.h

#include "ButtonWidget.h"

namespace mcp
{
    class SliderWidget final : public ButtonWidget
    {
        MCP_DEFINE_WIDGET(SliderWidget)

        float m_min;
        float m_max;
        float m_step; // TODO: Set this up.
        float m_value;
    public:
        SliderWidget(const WidgetConstructionData& data, const float min, const float max, const float step, const float startVal
            , LuaResourcePtr&& executeScript, LuaResourcePtr&& highlightScript, LuaResourcePtr&& pressReleaseScript);

        virtual bool PostLoadInit() override;
        [[nodiscard]] float GetValue() const { return m_value; }

        static SliderWidget* AddFromData(const XMLElement element);

    private:
        virtual void OnExecute([[maybe_unused]] const Vec2 relativeClickPosition) override;
        float GetValueNormalized() const;
    };
}