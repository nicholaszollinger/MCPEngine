#pragma once
// SelectionWidget.h

#include "Widget.h"

namespace mcp
{
    class ToggleWidget;

    struct SelectionWidgetConstructionData
    {
        WidgetConstructionData widgetData;
        LuaResourcePtr onValueChangedScript;
        size_t startVal = std::numeric_limits<size_t>::max();
        bool canBeNull = false;
    };

    class SelectionWidget final : public Widget
    {
        MCP_DEFINE_WIDGET(SelectionWidget)

        static constexpr size_t kInvalidSelection = std::numeric_limits<size_t>::max();

        std::string m_prefabPath;
        std::vector<ToggleWidget*> m_options;
        Script m_initializationScript;
        Script m_onValueChangedScript;
        size_t m_selection;
        bool m_canBeNull;

    public:
        SelectionWidget(SelectionWidgetConstructionData&& data);

        virtual bool Init() override;
        virtual bool PostLoadInit() override;
        void SetValue(const size_t value);
        [[nodiscard]] size_t GetValue() const;

        static SelectionWidget* AddFromData(const XMLElement element);
        static void RegisterLuaFunctions(lua_State* pState);

    private:
        void OnSelection(ToggleWidget* pToggle, const bool value);
        
        // DEBUG
        void PrintChildRect(const Widget* pWidget);
    };
}