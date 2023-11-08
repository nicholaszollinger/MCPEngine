// ToggleWidget.cpp

#include "ToggleWidget.h"

#include "LuaSource.h"
#include "MCP/Lua/Lua.h"

namespace mcp
{
    ToggleWidget::ToggleWidget(const WidgetConstructionData& data, const bool startVal, ButtonBehavior&& behavior)
        : ButtonWidget(data, std::move(behavior))
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

        auto behavior = GetButtonBehavior(element);

        return BLEACH_NEW(ToggleWidget(data, startVal, std::move(behavior)));
    }

    static int ScriptSetValue(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<ToggleWidget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);

        // Get the index
        const bool value = lua_toboolean(pState, -1);

        lua_pop(pState, 2);

        pWidget->SetValue(value);

        return 0;
    }

    void ToggleWidget::RegisterLuaFunctions(lua_State* pState)
    {
        static constexpr luaL_Reg kFuncs[]
        {
            {"SetValue", &ScriptSetValue}
            , {nullptr, nullptr}
        };

        // Get the global Widget library class,
        lua_getglobal(pState, "ToggleWidget");
        MCP_CHECK(lua_type(pState, -1) == LUA_TTABLE);

        // Set its functions
        luaL_setfuncs(pState, kFuncs, 0);

        // Pop the table off the stack.
        lua_pop(pState, 1);
    }
}
