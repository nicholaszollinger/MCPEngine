#pragma once
// ButtonWidget.h

#include "Widget.h"
#include "MCP/Lua/LuaSystem.h"

namespace mcp
{
    class KeyEvent;

    struct ButtonBehavior
    {
        LuaResourcePtr onExecuteScript;  
        LuaResourcePtr highlightBehaviorScript;
        LuaResourcePtr pressReleaseBehaviorScript;
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      I originally had the idea to add multicast delegates that anyone can add themselves to. But I would want to have
    //      Lua scripting more integrated for that to work.
    //		
    ///		@brief : Base ButtonWidget Component.
    //-----------------------------------------------------------------------------------------------------------------------------
    class ButtonWidget : public Widget
    {
        MCP_DEFINE_WIDGET(ButtonWidget)

    protected:
        LuaResourcePtr m_pOnExecuteScript;  
        LuaResourcePtr m_pHighlightBehaviorScript;
        LuaResourcePtr m_pPressReleaseBehaviorScript;

    private:
        bool m_isPressed = false;
        bool m_isHovered = false;

    public:
        ButtonWidget(const WidgetConstructionData& data, LuaResourcePtr&& onExecuteScript, LuaResourcePtr&& highlightBehaviorScript, LuaResourcePtr&& pressReleaseBehaviorScript);
        ButtonWidget(const WidgetConstructionData& data, ButtonBehavior&& behavior);
        virtual bool PostLoadInit() override;

        static ButtonWidget* AddFromData( const XMLElement element);

    protected:
        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when the button is executed.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnExecute([[maybe_unused]] const Vec2 relativeClickPosition);

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when the button is released; this happens only if the button was pressed.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnRelease([[maybe_unused]] const Vec2 relativeClickPosition);

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when the button is pressed. This is not executing the button! This is for any functionality you want
        ///             to do when a button is pressed.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnPress([[maybe_unused]] const Vec2 relativeClickPosition);

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when the button has input focus.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnHoverEnter();

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when the button leaves input focus.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnHoverExit();

        static ButtonBehavior GetButtonBehavior(const XMLElement element);
    private:
        virtual void HandleEvent(ApplicationEvent& event) override;
        void HandleMouseButtonPress(MouseButtonEvent& event);
        void HandleMouseMotion(MouseMoveEvent& event);
        virtual void HandleKeyPress(KeyEvent& event);

        virtual void OnInactive() override;
        // TODO: Handle keyboard and controller input. 
    };
}
