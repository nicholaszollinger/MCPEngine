#pragma once
// SelectionWidget.h

#include "Widget.h"

namespace mcp
{
    class ToggleWidget;

    class SelectionWidget final : public Widget
    {
        MCP_DEFINE_WIDGET(SelectionWidget)

        std::string m_prefabPath;
        std::vector<ToggleWidget*> m_options;
        LuaResourcePtr m_initializationScript;
        size_t m_selection;

    public:
        SelectionWidget(const WidgetConstructionData& data, const char* pPrefabPath, LuaResourcePtr&& initializationScript, const unsigned int startVal);

        virtual bool Init() override;
        virtual bool PostLoadInit() override;
        [[nodiscard]] size_t GetValue() const;

        static SelectionWidget* AddFromData(const XMLElement element);
    private:
        virtual void OnChildAdded(Widget* pChild) override;
        bool CreateOptionWidgets();
        void UpdateChildPositions() const;
        void OnSelection(ToggleWidget* pToggle, const bool value);
        // TODO:
        //virtual void OnChildRemoved() override;

        // DEBUG
        static void PrintChildRect(const Widget* pWidget);
    };
}