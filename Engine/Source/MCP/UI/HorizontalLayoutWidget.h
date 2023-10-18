#pragma once
// HorizontalLayoutWidget.h

#include "Widget.h"

namespace mcp
{


    class HorizontalLayoutWidget final : public Widget
    {
        MCP_DEFINE_WIDGET(HorizontalLayoutWidget)

    public:
        HorizontalLayoutWidget(const WidgetConstructionData& data);

        static HorizontalLayoutWidget* AddFromData(const XMLElement element);
    private:
        virtual void OnChildAdded(Widget* pChild) override;
        virtual void OnChildRemoved(Widget* pChild) override;

        void RecalculateChildRects();
    };
}