#pragma once
// ApplicationEvent.h

#include "Event.h"
#include "MCP/Debug/Assert.h"
#include "MCP/Input/InputCodes.h"
#include "Utility/Types/Vector2.h"

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Wrapper of the Event class to designate events coming from the Application. These things like Input events, Window events,
    ///         Quit events, etc.
    //-----------------------------------------------------------------------------------------------------------------------------
    class ApplicationEvent : public Event {};

    class ApplicationQuitEvent final : public ApplicationEvent
    {
        MCP_DEFINE_EVENT_ID(ApplicationQuitEvent)

    public:
        ApplicationQuitEvent() = default;
    };

    class WindowResizeEvent final : public ApplicationEvent
    {
        MCP_DEFINE_EVENT_ID(WindowResizeEvent)

        unsigned int m_width;
        unsigned int m_height;
    public:
        WindowResizeEvent(const unsigned int width, const unsigned int height)
            : m_width(width)
            , m_height(height)
        {
            //
        }

        [[nodiscard]] unsigned int GetWidth() const { return m_width; }
        [[nodiscard]] unsigned int GetHeight() const { return m_height; }
    };

    class WindowCloseEvent final : public ApplicationEvent
    {
        MCP_DEFINE_EVENT_ID(WindowResizeEvent)
    public:
        WindowCloseEvent() = default;
    };

    enum class ButtonState
    {
        kInvalid,
        kPressed,
        kHeld,
        kReleased,
    };

    class KeyEvent final : public ApplicationEvent
    {
        MCP_DEFINE_EVENT_ID(KeyEvent)

        MCPKey m_keycode  = MCPKey::kNull;
        ButtonState m_state  = ButtonState::kPressed;
        bool m_ctrl       = false;
        bool m_alt        = false;
        bool m_shift      = false;
    public:
        KeyEvent(const MCPKey keyCode, const ButtonState state, const bool ctrl, const bool alt, const bool shift)
            : m_keycode(keyCode)
            , m_state(state)
            , m_ctrl(ctrl)
            , m_alt(alt)
            , m_shift(shift)
        {
            //
        }

        [[nodiscard]] uint32_t GetGlyphValue() const;
        [[nodiscard]] MCPKey Key() const { return m_keycode; }
        [[nodiscard]] ButtonState State() const { return m_state; }
        [[nodiscard]] bool Ctrl() const { return m_ctrl; }
        [[nodiscard]] bool Alt() const { return m_alt; }
        [[nodiscard]] bool Shift() const { return m_shift; }
    };

    class MouseButtonEvent final : public ApplicationEvent
    {
        MCP_DEFINE_EVENT_ID(MouseButtonEvent)

        Vec2 m_windowPosition;
        int m_clicks = 0;
        float m_deltaTimeHeld = 0.f;
        MCPMouseButton m_button = MCPMouseButton::Invalid;
        ButtonState m_state  = ButtonState::kInvalid;

    public:
        MouseButtonEvent() = default;

        MouseButtonEvent(const MCPMouseButton button, const ButtonState state, const int clicks, const float x, const float y, const float deltaTimeHeld = 0.f)
            : m_windowPosition(x, y)
            , m_clicks(clicks)
            , m_deltaTimeHeld(deltaTimeHeld)
            , m_button(button)
            , m_state(state)
        {
            //
        }

        [[nodiscard]] float GetDeltaTimeHeld() const { return m_deltaTimeHeld; }
        [[nodiscard]] MCPMouseButton Button() const { return m_button; }
        [[nodiscard]] ButtonState State() const { return m_state; }
        [[nodiscard]] int NumClicks() const { return m_clicks; }
        [[nodiscard]] Vec2 GetWindowPosition() const { return m_windowPosition; }
    };

    class MouseMoveEvent final : public ApplicationEvent
    {
        MCP_DEFINE_EVENT_ID(MouseMoveEvent)

        Vec2 m_windowPosition;
    public:
        MouseMoveEvent(const float xPos, const float yPos)
            : m_windowPosition(xPos, yPos)
        {
            //
        }

        [[nodiscard]] Vec2 GetWindowPosition() const { return m_windowPosition; }
    };

    class MouseScrolledEvent final : public ApplicationEvent
    {
        MCP_DEFINE_EVENT_ID(MouseScrolledEvent)

        Vec2Int m_deltaPos;
    public:
        MouseScrolledEvent(const int xOffset, const int yOffset)
            : m_deltaPos(xOffset, yOffset)
        {
            //
        }

        [[nodiscard]] int GetDeltaX() const { return m_deltaPos.x; }
        [[nodiscard]] int GetDeltaY() const { return m_deltaPos.y; }
    };

}