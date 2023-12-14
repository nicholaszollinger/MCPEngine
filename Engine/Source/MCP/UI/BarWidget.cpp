// BarWidget.cpp

#include "BarWidget.h"

#include "LuaSource.h"
#include "MCP/Lua/Lua.h"

namespace mcp
{
    BarWidget::BarWidget(const WidgetConstructionData& data, const int min, const int max, const int step, const int startVal, LuaResourcePtr&& onValueChangedBehavior)
        : Widget(data)
        , m_onValueChangedScript(std::move(onValueChangedBehavior))
        , m_min(min)
        , m_max(max)
        , m_step(step)
        , m_value(startVal)
    {
        // Clamp the value just to be sure.
        m_value = std::clamp(m_value, m_min, m_max);
    }

    bool BarWidget::PostLoadInit()
    {
        const float percentage = GetValueNormalized();

        m_onValueChangedScript.Run("Init", this, percentage);

        return true;
    }

    void BarWidget::SetValue(int value)
    {
        if (value < m_min || value > m_max)
        {
            MCP_WARN("BarWidget", "Attempted to set value outside of bounds! Value: '", value, "'. Clamping to bounds");
            value = std::clamp(value, m_min, m_max);
        }

        m_value = value;

        // Let our script know.
        m_onValueChangedScript.Run("OnValueChanged", m_value, GetValueNormalized());
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the percentage of how full the bar is.
    ///		@returns : Value from [0, 1].
    //-----------------------------------------------------------------------------------------------------------------------------
    float BarWidget::GetValueNormalized() const
    {
        return static_cast<float>((m_value - m_min)) / static_cast<float>((m_max - m_min));
    }

    BarWidget* BarWidget::AddFromData(const XMLElement element)
    {
        // Get the construction data
        const auto data = GetWidgetConstructionData(element);

        // Get the Bar value data
        const auto min = element.GetAttributeValue<int>("min");
        const auto max = element.GetAttributeValue<int>("max");
        const auto step = element.GetAttributeValue<int>("step", 1);
        const auto startVal = element.GetAttributeValue<int>("startVal", max);

        // Get the OnValueChangedScript
        LuaResourcePtr script;
        const auto onValueChangedScript = element.GetChildElement("OnValueChangedScript");
        if (onValueChangedScript.IsValid())
        {
            const auto pScriptPath = onValueChangedScript.GetAttributeValue<const char*>("scriptPath");
            const auto pScriptDataPath = onValueChangedScript.GetAttributeValue<const char*>("scriptData");
            script = lua::LoadScriptInstance(pScriptPath, pScriptDataPath);
        }

        return BLEACH_NEW(BarWidget(data, min, max, step, startVal, std::move(script)));
    }

    static int GetBarWidgetMax(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<BarWidget*>(lua_touserdata(pState, -1));
        MCP_CHECK(pWidget);
        lua_pop(pState, 1);

        const auto max = pWidget->GetMax();
        lua_pushinteger(pState, max);

        return 1;
    }

    void BarWidget::RegisterLuaFunctions(lua_State* pState)
    {
        /*static constexpr luaL_Reg kFuncs[]
        {
            {"SetTint", &SetImageWidgetTint}
             ,{"GetChildBarWidget", &GetChildImageWidgetByTag}
             ,{"GetFirstChildImageWidget", &GetFirstChildImageWidget}
            ,{nullptr, nullptr}
        };*/

        static constexpr luaL_Reg kBarWidgetFuncs[]
        {
             {"GetMax", &GetBarWidgetMax}
            ,{nullptr, nullptr}
        };

        // Set the Widget Functions:
        //lua_getglobal(pState, "Widget");
        //MCP_CHECK(lua_type(pState, -1) == LUA_TTABLE);
        //luaL_setfuncs(pState, kFuncs, 0);

        //// Pop the table off the stack.
        //lua_pop(pState, 1);

        // Set the ImageWidget Functions:
        lua_getglobal(pState, "BarWidget");
        MCP_CHECK(lua_type(pState, -1) == LUA_TTABLE);
        luaL_setfuncs(pState, kBarWidgetFuncs, 0);

        // Pop the table off the stack.
        lua_pop(pState, 1);
    }

}