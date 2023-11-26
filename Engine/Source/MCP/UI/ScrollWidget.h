#pragma once
// ScrollWidget.h

#include "Widget.h"

namespace mcp
{
    class KeyEvent;
    class MouseButtonEvent;
    class MouseScrollEvent;

    class ScrollWidget final : public Widget
    {
    public:
        struct ScrollBehavior
        {
            float distanceOnKey = 1.f;
        };

    private:

        MCP_DEFINE_WIDGET(ScrollWidget)

        ScrollBehavior m_behavior;
        float m_maxLowerBound;
        //float m_scrollSpeed; TODO: For mouse controls

    public:
        ScrollWidget(const WidgetConstructionData& cData, const ScrollBehavior& scrollBehavior);

        void Scroll(const float distance) const;
        void SetDistanceOnKeyUpDown(const float distance);

        static ScrollWidget* AddFromData(const XMLElement element);
    private:
        virtual void HandleEvent(ApplicationEvent& event) override;
        virtual void OnChildAdded(Widget* pChild) override;
        void OnKeyEvent(KeyEvent& event);
        //TODO: void OnMouseEvent(MouseButtonEvent& event);
    };
}