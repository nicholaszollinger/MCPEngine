// SliderWidget.cpp

#include "SliderWidget.h"

#include "MCP/Lua/Lua.h"

namespace mcp
{
    SliderWidget::SliderWidget(const WidgetConstructionData& data, const float min, const float max, const float step, const float startVal, ButtonBehavior&& behavior)
        : ButtonWidget(data, std::move(behavior))
        , m_min(min)
        , m_max(max)
        , m_step(step)
        , m_value(startVal)
    {
        // Clamp the value just to be sure.
        m_value = std::clamp(m_value, m_min, m_max);
    }

    bool SliderWidget::PostLoadInit()
    {
        const float percentage = GetValueNormalized();

        // Initialize our script behavior.
        m_onExecuteScript.Run("Init", this, percentage);
        m_highlightScript.Run("Init", this, percentage);
        m_pressReleaseScript.Run("Init", this, percentage);

        return true;
    }

    void SliderWidget::OnExecute(const Vec2 relativeClickPosition)
    {
        // Based on the position of the click, I need to update the value of the slider.
        const auto rect = GetRect();

        const float percentage = relativeClickPosition.x / rect.width;
        m_value = m_min + (percentage * (m_max - m_min));

        // Call into the script implementation with the updated value.
        m_onExecuteScript.Run("OnExecute", percentage);
    }

    float SliderWidget::GetValueNormalized() const
    {
        return (m_value - m_min) / (m_max - m_min);
    }

    SliderWidget* SliderWidget::AddFromData(const XMLElement element)
    {
        // Get the construction data
        const auto data = GetWidgetConstructionData(element);

        // Get the slider data
        const auto min = element.GetAttributeValue<float>("min");
        const auto max = element.GetAttributeValue<float>("max");
        const auto step = element.GetAttributeValue<float>("step", 1.f);
        const auto startVal = element.GetAttributeValue<float>("startVal", max);

        // Get the button behavior.
        auto behavior = GetButtonBehavior(element);

        return BLEACH_NEW(SliderWidget(data, min, max, step, startVal, std::move(behavior)));
    }
}

