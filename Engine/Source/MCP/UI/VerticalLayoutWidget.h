#pragma once
// VerticalLayoutWidget.h
#include "Widget.h"

namespace mcp
{
    enum class Justification : uint32_t
    {
        kLeft = HashString32("left")
        , kCenter = HashString32("center")
        , kRight = HashString32("right")
    };

    class VerticalLayoutWidget final : public Widget
    {
        MCP_DEFINE_WIDGET(VerticalLayoutWidget)

        Justification m_justification;

    public:
        VerticalLayoutWidget(const WidgetConstructionData& data, const Justification justification);

        static VerticalLayoutWidget* AddFromData(const XMLElement element);
    private:
        virtual void OnChildAdded(Widget* pChild) override;
        virtual void OnChildRemoved(Widget* pChild) override;

        void RecalculateChildRects();
    };
}