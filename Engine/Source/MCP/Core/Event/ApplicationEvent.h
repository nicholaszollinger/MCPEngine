#pragma once
// ApplicationEvent.h

#include "MCP/Input/InputCodes.h"
#include "Utility/Generic/Hash.h"
#include "Utility/Types/Vector2.h"

namespace mcp
{
    using EventId = uint32_t;

    #define MCP_DEFINE_APPLICATION_EVENT_TYPE(type)                             \
    private:                                                                    \
        static inline const mcp::EventId kEventId = HashString32(#type);        \
    public:                                                                     \
        static mcp::EventId GetStaticId() { return kEventId; }                  \
        virtual mcp::EventId GetEventId() const override { return kEventId; }   \
        virtual const char* GetName() const override { return #type; }          \
    private:

    class ApplicationEvent
    {
        bool m_isHandled = false;

    public:
        ApplicationEvent() = default;
        virtual ~ApplicationEvent() = default;
        /*
        ApplicationEvent(const ApplicationEvent&) = default;
        ApplicationEvent(ApplicationEvent&&) = default;
        ApplicationEvent& operator=(const ApplicationEvent&) = default;
        ApplicationEvent& operator=(ApplicationEvent&&) = default;
        ~ApplicationEvent() = default;
        */

        [[nodiscard]] virtual const char* GetName() const = 0;
        [[nodiscard]] virtual EventId GetEventId() const = 0;
        [[nodiscard]] bool IsHandled() const { return m_isHandled; }
    };

    class WindowResizeEvent final : public ApplicationEvent
    {
        MCP_DEFINE_APPLICATION_EVENT_TYPE(WindowResizeEvent)

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
        MCP_DEFINE_APPLICATION_EVENT_TYPE(WindowResizeEvent)
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
        MCP_DEFINE_APPLICATION_EVENT_TYPE(KeyEvent)

        MCPKey m_keycode  = MCPKey::kInvalid;
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

        [[nodiscard]] MCPKey Key() const { return m_keycode; }
        [[nodiscard]] ButtonState State() const { return m_state; }
        [[nodiscard]] bool Ctrl() const { return m_ctrl; }
        [[nodiscard]] bool Alt() const { return m_alt; }
        [[nodiscard]] bool Shift() const { return m_shift; }
    };

    class MouseButtonEvent final : public ApplicationEvent
    {
        MCP_DEFINE_APPLICATION_EVENT_TYPE(MouseButtonEvent)

        Vec2Int m_windowPosition;
        int m_clicks;
        MCPMouseButton m_button;
        ButtonState m_state  = ButtonState::kInvalid;

    public:
        MouseButtonEvent(const MCPMouseButton button, const ButtonState state, const int clicks, const int x, const int y)
            : m_windowPosition(x, y)
            , m_clicks(clicks)
            , m_button(button)
            , m_state(state)
        {
            //
        }

        [[nodiscard]] MCPMouseButton Button() const { return m_button; }
        [[nodiscard]] ButtonState State() const { return m_state; }
        [[nodiscard]] int NumClicks() const { return m_clicks; }
        [[nodiscard]] Vec2Int GetWindowPosition() const { return m_windowPosition; }
    };

    class MouseMoveEvent final : public ApplicationEvent
    {
        MCP_DEFINE_APPLICATION_EVENT_TYPE(MouseMoveEvent)

        Vec2Int m_windowPosition;
    public:
        MouseMoveEvent(const int xPos, const int yPos)
            : m_windowPosition(xPos, yPos)
        {
            //
        }

        [[nodiscard]] Vec2Int GetWindowPosition() const { return m_windowPosition; }
    };

    class MouseScrolledEvent final : public ApplicationEvent
    {
        MCP_DEFINE_APPLICATION_EVENT_TYPE(MouseScrolledEvent)

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