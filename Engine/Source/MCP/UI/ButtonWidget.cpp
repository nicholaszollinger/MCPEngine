// ButtonWidget.cpp

#include "ButtonWidget.h"
#include "MCP/Core/Event/ApplicationEvent.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Lua/Lua.h"
#include "MCP/Scene/UILayer.h"

#if MCP_DEBUG_RENDER_BUTTON_BOUNDS
#include "MCP/Graphics/Graphics.h"
#endif

namespace mcp
{
    ButtonWidget::ButtonWidget(const WidgetConstructionData& data, LuaResourcePtr&& onExecuteScript, LuaResourcePtr&& highlightBehaviorScript, LuaResourcePtr&& pressReleaseBehaviorScript)
        : Widget(data)
#if MCP_DEBUG_RENDER_BUTTON_BOUNDS
        , IRenderable(RenderLayer::kDebugOverlay, 200)
#endif
        , m_pOnExecuteScript(std::move(onExecuteScript))
        , m_pHighlightBehaviorScript(std::move(highlightBehaviorScript))
        , m_pPressReleaseBehaviorScript(std::move(pressReleaseBehaviorScript))
    {
        //
    }

    ButtonWidget::ButtonWidget(const WidgetConstructionData& data, ButtonBehavior&& behavior)
        : Widget(data)
#if MCP_DEBUG_RENDER_BUTTON_BOUNDS
        , IRenderable(RenderLayer::kDebugOverlay, 200)
#endif
        , m_pOnExecuteScript(std::move(behavior.onExecuteScript))
        , m_pHighlightBehaviorScript(std::move(behavior.highlightBehaviorScript))
        , m_pPressReleaseBehaviorScript(std::move(behavior.pressReleaseBehaviorScript))
    {
        //
    }

    bool ButtonWidget::PostLoadInit()
    {
        // Initialize our script behavior.
        if (m_pOnExecuteScript.IsValid())
            lua::CallMemberFunction(m_pOnExecuteScript, "Init", this);

        if (m_pHighlightBehaviorScript.IsValid())
            lua::CallMemberFunction(m_pHighlightBehaviorScript, "Init", this);

        if (m_pPressReleaseBehaviorScript.IsValid())
            lua::CallMemberFunction(m_pPressReleaseBehaviorScript, "Init", this);
        return true;
    }

    void ButtonWidget::HandleEvent(ApplicationEvent& event)
    {
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

        // Key Press
        else if (eventId == KeyEvent::GetStaticId())
        {
            auto& key = static_cast<KeyEvent&>(event);
            HandleKeyPress(key);
        }
    }

    void ButtonWidget::HandleMouseButtonPress(MouseButtonEvent& event)
    {
        // If the button action happened within our Rect,
        const auto clickPosition = event.GetWindowPosition();
        Vec2 relativeClickPosition {};
        if (GetPointRelativeToRect(clickPosition, relativeClickPosition))
        {
            switch (event.State())
            {
                case ButtonState::kPressed:
                {
                    m_isPressed = true;
                    OnPress(relativeClickPosition);
                    break;
                }

                case ButtonState::kReleased:
                {
                    m_isPressed = false;
                    OnRelease(relativeClickPosition);
                    OnExecute(relativeClickPosition);
                    
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
            OnRelease(relativeClickPosition);

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
            
            OnHoverEnter();

            // We handled the event if it was inside our rect.
            event.SetHandled();
        }

        // If we were hovered, call the onHoverExit.
        else
        {
            if (!m_isHovered)
                return;

            OnHoverExit();
        }
    }

    void ButtonWidget::HandleKeyPress([[maybe_unused]] KeyEvent& event)
    {
        // TODO: Should this be navigation by default?
    }

    void ButtonWidget::OnChildAdded([[maybe_unused]] Widget* pChild)
    {
        Widget::OnChildAdded(pChild);
        
        if (m_sizedToContent && m_pParent)
        {
            GetParent()->OnChildSizeChanged();
        }
    }

    float ButtonWidget::GetRectWidth() const
    {
        // If we are sized to the content, then we are the total width of our children.
        if (m_sizedToContent)
        {
            float totalWidth = 0.f;
            for (const auto* pChild : m_children)
            {
                const auto* pWidget = SafeCastEntity<Widget>(pChild);

                totalWidth += pWidget->GetRectWidth();
            }

            return totalWidth;
        }

        // Otherwise return our set width.
        return GetScale().x * m_width;
    }

    float ButtonWidget::GetRectHeight() const
    {
        // If we are sized to the content, then we are the total height of our children.
        if (m_sizedToContent)
        {
            float totalHeight = 0.f;
            for (const auto* pChild : m_children)
            {
                const auto* pWidget = SafeCastEntity<Widget>(pChild);
                totalHeight += pWidget->GetRectHeight();
            }

            return totalHeight;
        }

        // Otherwise return our set height.
        return GetScale().y * m_height;
    }

    void ButtonWidget::OnInactive()
    {
#if MCP_DEBUG_RENDER_BUTTON_BOUNDS
        m_isVisible = false;
        m_pUILayer->RemoveRenderable(this);
#endif

        // If we were hovered, we need to return to the original state by 'un-hovering'
        if (m_isHovered)
            OnHoverExit();
    }

    void ButtonWidget::OnFocus()
    {
        // If the mouse is active, we need to check to see if the mouse is overlapping with our rect.
         // Check the position of the Mouse and determine if we need to Exit our over or not.
        const auto mousePos = GraphicsManager::Get()->GetWindow()->GetMousePosition();

        if (PointIntersectsRect(mousePos))
        {
            OnHoverEnter();
        }
    }

    void ButtonWidget::OnFocusLost()
    {
        // If we were hovered, we need to return to the original state by 'un-hovering'
        if (m_isHovered)
            OnHoverExit();
    }

    void ButtonWidget::OnExecute([[maybe_unused]] const Vec2 relativeClickPosition)
    {
        // MCP_LOG("ButtonWidget", "Calling OnPress");
        MCP_CHECK(m_pOnExecuteScript.IsValid());
        lua::CallMemberFunction(m_pOnExecuteScript, "OnExecute");
    }

    void ButtonWidget::OnPress([[maybe_unused]] const Vec2 relativeClickPosition)
    {
        if (m_pPressReleaseBehaviorScript.IsValid())
            lua::CallMemberFunction(m_pPressReleaseBehaviorScript, "OnPress");
    }

    void ButtonWidget::OnRelease([[maybe_unused]] const Vec2 relativeClickPosition)
    {
        if (m_pPressReleaseBehaviorScript.IsValid())
            lua::CallMemberFunction(m_pPressReleaseBehaviorScript, "OnRelease");
    }

    void ButtonWidget::OnHoverEnter()
    {
        m_isHovered = true;

        if (m_pHighlightBehaviorScript.IsValid())
            lua::CallMemberFunction(m_pHighlightBehaviorScript, "OnHoverEnter");
    }

    void ButtonWidget::OnHoverExit()
    {
        m_isHovered = false;

        if (m_pHighlightBehaviorScript.IsValid())
            lua::CallMemberFunction(m_pHighlightBehaviorScript, "OnHoverExit");
    }

    ButtonWidget* ButtonWidget::AddFromData(const XMLElement element)
    {
        auto data = GetWidgetConstructionData(element);
        data.isInteractable = true;

        auto behavior = GetButtonBehavior(element);

        return BLEACH_NEW(ButtonWidget(data, std::move(behavior)));
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the button behavior from a ButtonWidget or derived class's xml data.
    ///		@param element : The ButtonWidget element.
    //-----------------------------------------------------------------------------------------------------------------------------
    ButtonBehavior ButtonWidget::GetButtonBehavior(const XMLElement element)
    {
        ButtonBehavior behavior;

        const char* pScriptPath = nullptr;
        const char* pScriptDataPath = nullptr;

        // Execute Behavior
        auto scriptElement = element.GetChildElement("OnExecuteBehavior");
        if (scriptElement.IsValid())
        {
            pScriptPath = scriptElement.GetAttributeValue<const char*>("scriptPath");
            pScriptDataPath = scriptElement.GetAttributeValue<const char*>("scriptData");
            behavior.onExecuteScript = lua::LoadScriptInstance(pScriptPath, pScriptDataPath);
        }

        // Highlight Behavior
        pScriptPath = nullptr;
        scriptElement = element.GetChildElement("HighlightBehavior");
        if (scriptElement.IsValid())
        {
            pScriptPath = scriptElement.GetAttributeValue<const char*>("scriptPath");
            pScriptDataPath = scriptElement.GetAttributeValue<const char*>("scriptData");
            behavior.highlightBehaviorScript = lua::LoadScriptInstance(pScriptPath, pScriptDataPath);
        }

        // PressRelease Behavior
        pScriptPath = nullptr;
        scriptElement = element.GetChildElement("PressReleaseBehavior");
        if (scriptElement.IsValid())
        {
            pScriptPath = scriptElement.GetAttributeValue<const char*>("scriptPath");
            pScriptDataPath = scriptElement.GetAttributeValue<const char*>("scriptData");
            behavior.pressReleaseBehaviorScript = lua::LoadScriptInstance(pScriptPath, pScriptDataPath);
        }

        return behavior;
    }

#if MCP_DEBUG_RENDER_BUTTON_BOUNDS
    void ButtonWidget::OnActive()
    {
        m_isVisible = true;
        m_pUILayer->AddRenderable(this);
    }

    void ButtonWidget::Render() const
    {
        DrawRect(GetRectTopLeft(), Color::Black());
    }
#endif
}