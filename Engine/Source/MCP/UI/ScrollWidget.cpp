// ScrollWidget.cpp

#include "ScrollWidget.h"

#include "MCP/Core/Event/ApplicationEvent.h"

namespace mcp
{
    ScrollWidget::ScrollWidget(const WidgetConstructionData& cData, const ScrollBehavior& scrollBehavior)
        : Widget(cData)
        , m_behavior(scrollBehavior)
        , m_maxLowerBound(0.f)
    {
        m_sizedToContent = true;
    }

    void ScrollWidget::Scroll(const float distance) const
    {
        if (!HasChildren())
            return;

        // Get the child widget.
        // TODO: Should I enforce that this widget has a max number of children? I only operate on one child for this widget.
        auto* pChild = SafeCastEntity<Widget>(m_children[0]);
        auto currentPos = pChild->GetLocalPosition();

        // Add the distance, but clamp the offset so that you can not go past the boundaries of the child.
        currentPos.y += distance;
        currentPos.y = std::clamp(currentPos.y, m_maxLowerBound,  0.f);

        // Move its local position.
        pChild->SetLocalPosition(currentPos.x, currentPos.y);
    }

    void ScrollWidget::HandleEvent(ApplicationEvent& event)
    {
        if (event.GetEventId() == KeyEvent::GetStaticId())
        {
            auto keyEvent = static_cast<KeyEvent&>(event);
            OnKeyEvent(keyEvent);
        }
    }

    void ScrollWidget::OnChildAdded(Widget* pChild)
    {
        Widget::OnChildAdded(pChild);

        pChild->SetAnchor(0.5f, 0.f);
        pChild->SetPivot(0.5f, 0.f);

        m_maxLowerBound = std::abs(pChild->GetRectHeight() - GetRectHeight()) * -0.5f;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the Distance that the scroll widget moves on a single "Move Up/Down One" action (A single press of the up or down arrow keys).
    //-----------------------------------------------------------------------------------------------------------------------------
    void ScrollWidget::SetDistanceOnKeyUpDown(const float distance)
    {
        m_behavior.distanceOnKey = std::abs(distance);
    }

    void ScrollWidget::OnKeyEvent(KeyEvent& event)
    {
        if (event.State() == ButtonState::kPressed || event.State() == ButtonState::kHeld)
        {
            // TODO: Each of these should not be specific keys, but 'InputActions'. A 'MoveUp/DownOne' action and 'JumpUp/Down' action
            switch (event.Key())
            {
                // Snap Up by one child element.
                case MCPKey::Up:
                {
                    Scroll(m_behavior.distanceOnKey);
                    event.SetHandled();
                    break;
                }

                // Snap Down by one child element.
                case MCPKey::Down:
                {
                    Scroll(-m_behavior.distanceOnKey);
                    event.SetHandled();
                    break;
                }

                // Move Up by one 'mask height'.
                case MCPKey::PageUp:
                {
                    Scroll(GetRectHeight());
                    event.SetHandled();
                    break;
                }

                // Move Down by one 'mask height'.
                case MCPKey::PageDown:
                {
                    Scroll(-GetRectHeight());
                    event.SetHandled();
                    break;
                }

                default: break;
            }
        }
    }

    ScrollWidget* ScrollWidget::AddFromData(const XMLElement element)
    {
        WidgetConstructionData data = GetWidgetConstructionData(element);

        ScrollBehavior scrollBehavior;

        // Scroll Behavior:
        const auto scrollElement = element.GetChildElement("ScrollBehavior");
        MCP_CHECK(scrollElement.IsValid());

        scrollBehavior.distanceOnKey = scrollElement.GetAttributeValue<float>("distanceOnKey", 5.f);

        return BLEACH_NEW(ScrollWidget(data, scrollBehavior));
    }
}
