#pragma once
// ButtonWidget.h

#include "Widget.h"
#include "MCP/Lua/LuaSystem.h"

#define MCP_DEBUG_RENDER_BUTTON_BOUNDS 0

#ifndef _DEBUG
#define MCP_DEBUG_RENDER_BUTTON_BOUNDS 0
#endif

#if MCP_DEBUG_RENDER_BUTTON_BOUNDS
#include "MCP/Scene/IRenderable.h"
#endif

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
#if MCP_DEBUG_RENDER_BUTTON_BOUNDS
        , public IRenderable
#endif
    {
        MCP_DEFINE_WIDGET(ButtonWidget)

    protected:
        Script m_onExecuteScript;  
        Script m_highlightScript;
        Script m_pressReleaseScript;

    private:
        bool m_isPressed = false;
        bool m_isHovered = false;

    public:
        ButtonWidget(const WidgetConstructionData& data, LuaResourcePtr&& onExecuteScript, LuaResourcePtr&& highlightBehaviorScript, LuaResourcePtr&& pressReleaseBehaviorScript);
        ButtonWidget(const WidgetConstructionData& data, ButtonBehavior&& behavior);
        ButtonWidget(const WidgetConstructionData& data, const ButtonBehavior& behavior);
        virtual bool PostLoadInit() override;

        //void SetButtonBehavior(const ButtonBehavior&);
        [[nodiscard]] virtual float GetRectWidth() const override;
        [[nodiscard]] virtual float GetRectHeight() const override;

        static ButtonWidget* AddFromData( const XMLElement element);
        static ButtonBehavior GetButtonBehavior(const XMLElement element);

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

        virtual void OnMove() override;

    private:
        virtual void OnChildAdded(Widget* pChild) override;
        virtual void HandleEvent(ApplicationEvent& event) override;
        void HandleMouseButtonPress(MouseButtonEvent& event);
        void HandleMouseMotion(MouseMoveEvent& event);
        virtual void HandleKeyPress(KeyEvent& event);

        virtual void OnInactive() override;
        virtual void OnFocus() override;
        virtual void OnFocusLost() override;
        // TODO: Handle keyboard and controller input.
        
#if MCP_DEBUG_RENDER_BUTTON_BOUNDS
    public:
        virtual void OnActive() override;
        virtual void Render() const override;
    private:
#endif
    };
}
