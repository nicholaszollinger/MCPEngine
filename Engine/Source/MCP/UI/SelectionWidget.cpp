// SelectionWidget.cpp

#include "SelectionWidget.h"

#include "ToggleWidget.h"
#include "MCP/Lua/Lua.h"
#include "MCP/Scene/UILayer.h"

namespace mcp
{
    SelectionWidget::SelectionWidget(const WidgetConstructionData& data, const char* pPrefabPath, LuaResourcePtr&& initializationScript,  const unsigned startVal)
        : Widget(data)
        , m_prefabPath(pPrefabPath)
        , m_initializationScript(std::move(initializationScript))
        , m_selection(startVal)
    {
        //
    }

    bool SelectionWidget::Init()
    {
        // Create the Options
        if (!CreateOptionWidgets())
            return false;

        return true;
    }

    bool SelectionWidget::PostLoadInit()
    {
        // Update the child positions:
        UpdateChildPositions();

        m_options[m_selection]->SetValue(true);

        return true;
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
    ///		@brief : When a child is added to our selection widget, we need to find the Toggle Widget and add it to our list of
    ///         options.
    ///		@param pChild : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void SelectionWidget::OnChildAdded(Widget* pChild)
    {
        ToggleWidget* pToggle = nullptr;
        // I need to listen to the ToggleButton's OnValueChanged.
        if (pChild->GetTypeId() == ToggleWidget::GetStaticTypeId())
        {
            pToggle = static_cast<ToggleWidget*>(pChild);
        }

        else
        {
            pToggle = pChild->FindFirstChildOfType<ToggleWidget>();
        }

        // Return if there isn't any child ToggleWidget.
        //MCP_CHECK_MSG(pToggle, "Failed to find ToggleWidget on Child!");
        if (!pToggle)
            return;

        pToggle->m_onValueChanged.AddListener(this, [this](ToggleWidget* pToggleWidget, const bool value) { this->OnSelection(pToggleWidget, value); });

        // Add the Toggle to our array
        m_options.emplace_back(pToggle);
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
            // If we are trying to toggle the current selection off, re-toggle it
            if (pToggle == m_options[m_selection])
                pToggle->SetValue(true);

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

        // Set our old option to false.
        m_options[oldSelection]->SetValue(false);

        MCP_LOG("SelectionWidget", "Selected: ", m_selection);
    }

    void SelectionWidget::PrintChildRect(const Widget* pWidget)
    {
        MCP_LOG("Selection Child", "Rect: ", pWidget->GetRect().ToString());

        for (auto* pChild : pWidget->GetChildren())
        {
            PrintChildRect(pChild);
        }
    }

    void SelectionWidget::UpdateChildPositions() const
    {
        if (m_children.empty())
            return;

        const auto myRect = GetRect();
        const float distance = myRect.width / static_cast<float>(m_children.size());

        for (size_t i = 0; i < m_children.size(); ++i)
        {
            m_children[i]->SetAnchor(0.f, 0.5f);
            m_children[i]->SetLocalPosition((static_cast<float>(i) * distance) + (distance / 2.f), 0.f);
        }

        //PrintChildRect(this);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Create our options widgets using the prefab. On failure, it returns false.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool SelectionWidget::CreateOptionWidgets()
    {
        // Load the Prefab:
        XMLParser parser;
        if (!parser.LoadFile(m_prefabPath.c_str()))
        {
            MCP_ERROR("SelectionWidget", "Failed create options! Failed open OptionPrefab at path: ", m_prefabPath);
            return false;
        }

        // Get the Prefab Root.
        const auto prefabRootElement = parser.GetElement("Widget");
        if (!prefabRootElement.IsValid())
        {
            MCP_ERROR("SelectionWidget", "Failed create options! Failed find Options element!");
            parser.CloseCurrentFile();
            return false;
        }

        // Get the Options Root.
        const auto optionsElement = prefabRootElement.GetSiblingElement("Options");
        if (!optionsElement.IsValid())
        {
            MCP_ERROR("SelectionWidget", "Failed to add from data! Failed find Options element!");
            parser.CloseCurrentFile();
            return false;
        }

        // Create a table for holding options data.
        LuaResourcePtr optionsData = lua::CreateTable();

        // Add the Options
        auto option = optionsElement.GetChildElement("Option");
        while(option.IsValid())
        {
            auto var = option.GetChildElement("Var");
            while (var.IsValid())
            {
                // Get the element data:
                std::string pType = var.GetAttributeValue<const char*>("type", "string");
                const char* pKey = var.GetAttributeValue<const char*>("key");
                const char* pValue = var.GetAttributeValue<const char*>("value");

                if (pType == "string")
                {
                    lua::SetElementInTable(optionsData, pKey, pValue);
                }

                // TODO: Other table variables.
                else
                {
                    MCP_ERROR("SelectionWidget", "Failed create option! Unhandled variable type!");
                }

                var = var.GetSiblingElement("Var");
            }

            // Create a new Widget based on the OptionPrefab.
            auto* pOptionWidget = GetLayer()->CreateWidgetFromData(prefabRootElement);

            // Initialize the Prefab with the data.
            lua::CallMemberFunction(m_initializationScript, "SetOptionData", pOptionWidget, optionsData);

            // Add this as a child of this widget.
            AddChild(pOptionWidget);

            // Continue to the next option.
            option = option.GetSiblingElement("Option");
        }

        // If we don't have any options, return.
        if (m_options.empty())
            return true;

        // Set our start val:
        if (m_selection >= m_options.size())
        {
            MCP_WARN("SelectionWidget", "Starting value was out of bounds for options. Starting Value: ", m_selection, ". Setting to start val...");
            m_selection = 0;
        }

        return true;
    }

    SelectionWidget* SelectionWidget::AddFromData(const XMLElement element)
    {
        const auto data = GetWidgetConstructionData(element);
        const unsigned int startVal = element.GetAttributeValue<int>("startVal", 0);

        // Get the PrefabElement:
        const auto prefabElement = element.GetChildElement("OptionPrefab");
        if (!prefabElement.IsValid())
        {
            MCP_ERROR("SelectionWidget", "Failed to add from data! Failed to find OptionPrefab!");
            return nullptr;
        }

        // Get the path to the Prefab
        const char* pPrefabPath = prefabElement.GetAttributeValue<const char*>("path");
        MCP_CHECK(pPrefabPath);

        // Get the OptionInitializationElement.
        const auto optionInitializationElement = prefabElement.GetSiblingElement("OptionInitializationScript");
        if (!optionInitializationElement.IsValid())
        {
            MCP_ERROR("SelectionWidget", "Failed to add from data! Failed to find OptionInitializationScript element!");
            return nullptr;
        }

        // Load the initialization script.
        const auto path = optionInitializationElement.GetAttributeValue<const char*>("scriptPath");
        MCP_CHECK(path);
        LuaResourcePtr initializationScript = lua::LoadScriptInstance(path);
        if (!initializationScript.IsValid())
        {
            MCP_ERROR("SelectionWidget", "Failed to add from data! Failed to load InitializationScropt from path: ", path);
            return nullptr;
        }

        // Create the selection Widget.
        return BLEACH_NEW(SelectionWidget(data, pPrefabPath, std::move(initializationScript), startVal));
    }

}
