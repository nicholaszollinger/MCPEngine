// ToggleWidget.cpp

#include "ToggleWidget.h"

#include "MCP/Lua/Lua.h"

namespace mcp
{
    ToggleWidget::ToggleWidget(const WidgetConstructionData& data, const bool startVal, LuaResourcePtr&& onExecuteScript, LuaResourcePtr&& highlightBehaviorScript, LuaResourcePtr&& pressReleaseBehaviorScript)
        : ButtonWidget(data, std::move(onExecuteScript), std::move(highlightBehaviorScript), std::move(pressReleaseBehaviorScript))
        , m_value(startVal)
    {
        //
    }

    bool ToggleWidget::PostLoadInit()
    {
        //MCP_LOG("ToggleWidget", "Rect: ", GetScreenRect().ToString());

        // Initialize our script behavior.
        if (m_pOnExecuteScript.IsValid())
            lua::CallMemberFunction(m_pOnExecuteScript, "Init", this, m_value);

        if (m_pHighlightBehaviorScript.IsValid())
            lua::CallMemberFunction(m_pHighlightBehaviorScript, "Init", this, m_value);

        if (m_pPressReleaseBehaviorScript.IsValid())
            lua::CallMemberFunction(m_pPressReleaseBehaviorScript, "Init", this, m_value);

        return true;
    }

    void ToggleWidget::OnExecute([[maybe_unused]] const Vec2 relativeClickPosition)
    {
        m_value = !m_value;

        m_onValueChanged.Broadcast(this, m_value);
        if (m_pOnExecuteScript.IsValid())
        {
            lua::CallMemberFunction(m_pOnExecuteScript, "OnExecute", m_value);
        }
    }

    void ToggleWidget::SetValue(const bool value)
    {
        if (m_value == value)
            return;

        m_value = value;
        m_onValueChanged.Broadcast(this, m_value);

        if (m_pOnExecuteScript.IsValid())
        {
            lua::CallMemberFunction(m_pOnExecuteScript, "OnExecute", m_value);
        }
    }

    ToggleWidget* ToggleWidget::AddFromData(const XMLElement element)
    {
        const auto data = GetWidgetConstructionData(element);
        const bool startVal = element.GetAttributeValue<bool>("startVal");

        LuaResourcePtr executeScript;
        LuaResourcePtr highlightScript;
        LuaResourcePtr pressReleaseScript;

        // Toggle Behavior
        auto scriptElement = element.GetChildElement("ToggleBehavior");
        if (!scriptElement.IsValid())
        {
            MCP_ERROR("ToggleWidget", "Failed to find ToggleBehavior script!");
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

        return BLEACH_NEW(ToggleWidget(data, startVal, std::move(executeScript),std::move(highlightScript), std::move(pressReleaseScript)));
    }
}
