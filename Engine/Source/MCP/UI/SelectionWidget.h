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
        LuaResourcePtr m_onValueChangedScript;
        size_t m_selection;

    public:
        SelectionWidget(const WidgetConstructionData& data, const char* pPrefabPath, LuaResourcePtr&& initializationScript, LuaResourcePtr&& onValueChangedScript, const unsigned int startVal);

        virtual bool Init() override;
        virtual bool PostLoadInit() override;
        void SetValue(const size_t value);
        [[nodiscard]] size_t GetValue() const;

        static SelectionWidget* AddFromData(const XMLElement element);
        static void RegisterLuaFunctions(lua_State* pState);

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