#pragma once
// ButtonWidget.h

#include "Widget.h"
#include "MCP/Scene/IRenderable.h"

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      I originally had the idea to add multicast delegates that anyone can add themselves to. But I would want to have
    //      Lua scripting more integrated for that to work.
    //		
    ///		@brief : Base ButtonWidget Component.
    //-----------------------------------------------------------------------------------------------------------------------------
    class ButtonWidget : public Widget
    {
        //MCP_DEFINE_WIDGET(ButtonWidget)
    private:
        bool m_isPressed = false;
        bool m_isHovered = false;

    public:
        ButtonWidget(const WidgetConstructionData& data);
        //static bool AddFromData([[maybe_unused]] const XMLElement element) { return false; }

    protected:
        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when the button is executed.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnExecute() {}

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when the button is released; this happens only if the button was pressed.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnRelease() {}

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when the button is pressed. This is not executing the button! This is for any functionality you want
        ///             to do when a button is pressed.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnPress() {}

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when the button has input focus.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnHoverEnter() {}

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when the button leaves input focus.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnHoverExit() {}

    private:
        virtual void HandleEvent(ApplicationEvent& event) override;
        void HandleMouseButtonPress(MouseButtonEvent& event);
        void HandleMouseMotion(MouseMoveEvent& event);
        // TODO: Handle keyboard and controller input. 
    };
}