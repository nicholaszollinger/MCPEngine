// SliderWidget.cpp

#include "SliderWidget.h"

#include "MCP/Lua/Lua.h"


namespace mcp
{
    SliderWidget::SliderWidget(const WidgetConstructionData& data, const float min, const float max, const float step, const float startVal
        , LuaResourcePtr&& executeScript, LuaResourcePtr&& highlightScript, LuaResourcePtr&& pressReleaseScript)
        : ButtonWidget(data, std::move(executeScript), std::move(highlightScript), std::move(pressReleaseScript))
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
        if (m_pOnExecuteScript.IsValid())
            lua::CallMemberFunction(m_pOnExecuteScript, "Init", this, percentage);

        if (m_pHighlightBehaviorScript.IsValid())
            lua::CallMemberFunction(m_pHighlightBehaviorScript, "Init", this, percentage);

        if (m_pPressReleaseBehaviorScript.IsValid())
            lua::CallMemberFunction(m_pPressReleaseBehaviorScript, "Init", this, percentage);

        return true;
    }

    void SliderWidget::OnExecute(const Vec2 relativeClickPosition)
    {
        // Based on the position of the click, I need to update the value of the slider.
        const auto rect = GetRect();

        const float percentage = relativeClickPosition.x / rect.width;
        m_value = m_min + (percentage * (m_max - m_min));

        // Call into the script implementation with the updated value.
        if (m_pOnExecuteScript.IsValid())
            lua::CallMemberFunction(m_pOnExecuteScript, "OnExecute", percentage);
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

        // Get the Script data:
        LuaResourcePtr executeScript;
        LuaResourcePtr highlightScript;
        LuaResourcePtr pressReleaseScript;

        // Slider Behavior
        auto scriptElement = element.GetChildElement("SliderBehavior");
        if (!scriptElement.IsValid())
        {
            MCP_ERROR("SliderWidget", "Failed to load from data! Failed to find SliderBehavior script!");
            return nullptr;
        }

        const char* pScriptPath = scriptElement.GetAttributeValue<const char*>("scriptPath");
        const char* pScriptDataPath = scriptElement.GetAttributeValue<const char*>("scriptData");
        executeScript = lua::LoadScriptInstance(pScriptPath, pScriptDataPath);

        // Highlight Behavior
        pScriptPath = nullptr;
        scriptElement = scriptElement.GetSiblingElement("HighlightBehavior");
        if (scriptElement.IsValid())
        {
            pScriptPath = scriptElement.GetAttributeValue<const char*>("scriptPath");
            pScriptDataPath = scriptElement.GetAttributeValue<const char*>("scriptData");
            highlightScript = lua::LoadScriptInstance(pScriptPath, pScriptDataPath);
        }

        // PressRelease Behavior
        pScriptPath = nullptr;
        scriptElement = scriptElement.GetSiblingElement("PressReleaseBehavior");
        if (scriptElement.IsValid())
        {
            pScriptPath = scriptElement.GetAttributeValue<const char*>("scriptPath");
            pScriptDataPath = scriptElement.GetAttributeValue<const char*>("scriptData");
            pressReleaseScript = lua::LoadScriptInstance(pScriptPath, pScriptDataPath);
        }

        return BLEACH_NEW(SliderWidget(data, min, max, step, startVal, std::move(executeScript), std::move(highlightScript), std::move(pressReleaseScript)));
    }
}

