// ButtonWidget.cpp

#include "ButtonWidget.h"
#include "MCP/Core/Event/ApplicationEvent.h"

namespace mcp
{
    ButtonWidget::ButtonWidget(const WidgetConstructionData& data)
        : Widget(data)
    {
        //
    }

    void ButtonWidget::HandleEvent(ApplicationEvent& event)
    {
        // If I or the parent is not active, don't handle the event.
        if (!m_isActive)
            return;

        const auto eventId = event.GetEventId();

        // Mouse Button Press
        if (eventId == MouseButtonEvent::GetStaticId())
        {
            auto& button = static_cast<MouseButtonEvent&>(event);
            HandleMouseButtonPress(button);
        }

        // Mouse Motion
        else if (eventId == MouseMoveEvent::GetStaticId())
        {
            auto& motion = static_cast<MouseMoveEvent&>(event);
            HandleMouseMotion(motion);
        }
    }

    void ButtonWidget::HandleMouseButtonPress(MouseButtonEvent& event)
    {
        // If the button action happened within our Rect,
        if (PointIntersectsRect(event.GetWindowPosition()))
        {
            switch (event.State())
            {
                case ButtonState::kPressed:
                {
                    m_isPressed = true;
                    OnPress();
                    break;
                }

                case ButtonState::kReleased:
                {
                    m_isPressed = false;
                    OnRelease();
                    OnExecute();
                    
                    break;
                }

                default: break;
            }

            // Set the event as handled.
            event.SetHandled();
        }

        // If the event happened outside of our rect, but we were pressed and now are released, then release the button
        // without executing.
        else if (m_isPressed && event.State() == ButtonState::kReleased)
        {
            m_isPressed = false;
            OnRelease();

            // Set the event as handled.
            event.SetHandled();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Respond to mouse motion by seeing if we need to be hovered or not.
    ///		@param event : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void ButtonWidget::HandleMouseMotion(MouseMoveEvent& event)
    {
        // If the button action happened within our Rect,
        if (PointIntersectsRect(event.GetWindowPosition()))
        {
            if (m_isHovered)
                return;

            m_isHovered = true;
            OnHoverEnter();

            // We handled the event if it was inside our rect.
            event.SetHandled();
        }

        // If we were hovered, call the onHoverExit.
        else
        {
            if (!m_isHovered)
                return;

            m_isHovered = false;
            OnHoverExit();
            //m_onHoverExit.Broadcast();
        }
    }

}