// ToggleWidget.cpp

#include "ToggleWidget.h"

#include "LuaSource.h"
#include "MCP/Graphics/Graphics.h"
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
        // Initialize our script behavior.
        m_onExecuteScript.Run("Init", this, m_value);
        m_highlightScript.Run("Init", this, m_value);
        m_pressReleaseScript.Run("Init", this, m_value);

        return true;
    }

    void ToggleWidget::OnExecute([[maybe_unused]] const Vec2 relativeClickPosition)
    {
        m_value = !m_value;

        m_onValueChanged.Broadcast(this, m_value);
        m_onExecuteScript.Run("OnExecute", m_value);
    }

    void ToggleWidget::SetValue(const bool value)
    {
        if (m_value == value)
            return;

        m_value = value;
        m_onValueChanged.Broadcast(this, m_value);
        m_onExecuteScript.Run("OnExecute", m_value);

        // If our value was set outside of an input event, we need to exit our hover state.
        const auto mousePos = GraphicsManager::Get()->GetWindow()->GetMousePosition();
        const bool intersects = PointIntersectsRect(mousePos);
        if (!m_value && !intersects)
        {
            OnHoverExit();
        }

        else if (m_value && intersects)
        {
            OnHoverEnter();
        }
    }

    ToggleWidget* ToggleWidget::AddFromData(const XMLElement element)
    {
        const auto data = GetWidgetConstructionData(element);
        const bool startVal = element.GetAttributeValue<bool>("startVal");

        auto behavior = GetButtonBehavior(element);

        return BLEACH_NEW(ToggleWidget(data, startVal, std::move(behavior)));
    }

    static int ScriptSetToggleValue(lua_State* pState)
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

    static int ScriptGetToggleValue(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<ToggleWidget*>(lua_touserdata(pState, -1));
        MCP_CHECK(pWidget);

        lua_pop(pState, 1);

        lua_pushboolean(pState, pWidget->GetValue());

        return 1;
    }

    void ToggleWidget::RegisterLuaFunctions(lua_State* pState)
    {
        static constexpr luaL_Reg kFuncs[]
        {
            {"SetValue", &ScriptSetToggleValue}
            ,{"GetValue", &ScriptGetToggleValue}
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
