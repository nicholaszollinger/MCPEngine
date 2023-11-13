// SelectionWidget.cpp

#include "SelectionWidget.h"

#include "LuaSource.h"
#include "ToggleWidget.h"
#include "LuaSource/lstate.h"
#include "MCP/Lua/Lua.h"
#include "MCP/Scene/UILayer.h"

namespace mcp
{
    SelectionWidget::SelectionWidget(SelectionWidgetConstructionData&& data)
        : Widget(data.widgetData)
        , m_onValueChangedScript(std::move(data.onValueChangedScript))
        , m_selection(data.startVal)
        , m_canBeNull(data.canBeNull)
    {
        //
    }

    bool SelectionWidget::Init()
    {
        return true;
    }

    bool SelectionWidget::PostLoadInit()
    {
        // Find all toggles that we have as children and add them to our options.
        m_options = GetAllChildrenOfType<ToggleWidget>();

        for (auto* pToggle : m_options)
        {
            // Set its value to false.
            pToggle->SetValue(false);

            // Then listen for updates
            pToggle->m_onValueChanged.AddListener(this, [this](ToggleWidget* pToggleWidget, const bool value) { this->OnSelection(pToggleWidget, value); });
        }

        // Initialize our script:
        if (m_onValueChangedScript.IsValid())
        {
            lua::CallMemberFunction(m_onValueChangedScript, "Init", this);
        }

        // If we have a start value, set it.
        if (m_selection == kInvalidSelection)
        {
            if (m_onValueChangedScript.IsValid())
                lua::CallMemberFunction(m_onValueChangedScript, "OnValueNull");
        }

        else
        {
            MCP_CHECK(m_selection < m_options.size());
            m_options[m_selection]->SetValue(true);
        }

        return true;
    }

    void SelectionWidget::SetValue(const size_t value)
    {
        MCP_CHECK(value < m_options.size());
        m_selection = value;

        OnSelection(m_options[m_selection], true);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Returns the index of the currently selected value.
    //-----------------------------------------------------------------------------------------------------------------------------
    size_t SelectionWidget::GetValue() const
    {
        MCP_CHECK(m_selection < m_options.size());
        return m_selection;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Respond to one of our ToggleWidgets being toggled. We need to make sure that we update our current selection.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SelectionWidget::OnSelection(ToggleWidget* pToggle, const bool value)
    {
        MCP_CHECK(pToggle);

        // If we are responding to a toggle being turned off,
        if (!value)
        {
            // If that toggle is our current selection,
            if (pToggle == m_options[m_selection])
            {
                // If we can be null, allow it to be inactive, and let our script know:
                if (m_canBeNull)
                {
                    m_selection = kInvalidSelection;
                    if (m_onValueChangedScript.IsValid())
                        lua::CallMemberFunction(m_onValueChangedScript, "OnValueNull");
                }

                // If we can't be null, then re-toggle it.
                else
                {
                    pToggle->SetValue(true);
                }
            }

            return;
        }

        // Store our previous selection:
        const size_t oldSelection = m_selection;
        m_selection = m_options.size();

        // Find our new selected value.
        for (size_t i = 0; i < m_options.size(); ++i)
        {
            if (m_options[i] == pToggle)
            {
                // Case where we are re-toggling our the current selection. (This happens as a consequence from toggling the current
                // selection off, we have to re-toggle it).
                if (oldSelection == i)
                {
                    m_selection = oldSelection;
                    return;
                }

                m_selection = i;
                break;
            }
        }

        // Case where the Toggle doesn't not exist in our options array:
        if (m_selection == m_options.size())
        {
            MCP_ERROR("SelectionWidget", "Made a selection that doesn't exist in the array of options! Setting the previous option on.");
            m_selection = oldSelection;
            m_options[m_selection]->SetValue(true);
            return;
        }

        // If we had an old selection, then set it to false.
        if (oldSelection != kInvalidSelection)
        {
            m_options[oldSelection]->SetValue(false);
        }

        // Have the script respond to the change.
        if (m_onValueChangedScript.IsValid())
        {
            lua::CallMemberFunction(m_onValueChangedScript, "OnValueChanged", m_selection);
        }
    }

    void SelectionWidget::PrintChildRect([[maybe_unused]] const Widget* pWidget)
    {
        MCP_LOG("Selection Child", "Rect: ", pWidget->GetRect().ToString());

        for (auto* pChild : m_children)
        {
            auto* pWidgetChild = SafeCastEntity<Widget>(pChild);
            PrintChildRect(pWidgetChild);
        }
    }

    SelectionWidget* SelectionWidget::AddFromData(const XMLElement element)
    {
        SelectionWidgetConstructionData data;

        data.widgetData = GetWidgetConstructionData(element);
        data.startVal = element.GetAttributeValue<size_t>("startVal", kInvalidSelection);
        data.canBeNull = element.GetAttributeValue<bool>("canBeNull", false);

        // Get the OnValueChangedScript
        const auto onValueChangedElement = element.GetChildElement("OnValueChanged");
        if (onValueChangedElement.IsValid())
        {
            const auto path = onValueChangedElement.GetAttributeValue<const char*>("scriptPath");
            const auto dataPath = onValueChangedElement.GetAttributeValue<const char*>("scriptData", nullptr);
            data.onValueChangedScript = lua::LoadScriptInstance(path, dataPath);
        }

        // Create the selection Widget.
        return BLEACH_NEW(SelectionWidget(std::move(data)));
    }

    static int ScriptSetSelectionValue(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<SelectionWidget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);

        // Get the index
        const auto index = lua_tointeger(pState, -1);

        lua_pop(pState, 2);

        pWidget->SetValue(index);

        return 0;
    }

    void SelectionWidget::RegisterLuaFunctions(lua_State* pState)
    {
        static constexpr luaL_Reg kFuncs[]
        {
            {"SetValue", &ScriptSetSelectionValue}
            , {nullptr, nullptr}
        };

        // Get the global Widget library class,
        lua_getglobal(pState, "SelectionWidget");
        MCP_CHECK(lua_type(pState, -1) == LUA_TTABLE);

        // Set its functions
        luaL_setfuncs(pState, kFuncs, 0);

        // Pop the table off the stack.
        lua_pop(pState, 1);
    }

}
