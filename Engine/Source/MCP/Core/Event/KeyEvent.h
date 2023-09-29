#pragma once
// KeyEvent.h

#include "Event.h"
#include "MCP/Input/InputCodes.h"

namespace mcp
{
    //enum class ButtonState
    //{
    //    kInvalid    = 0,
    //    kPressed    = MCP_BIT(0),
    //    kHeld       = MCP_BIT(1),
    //    kReleased   = MCP_BIT(2),
    //};

    //struct KeyEvent final : public Event
    //{
    //    MCPKey keycode  = MCPKey::kInvalid;
    //    ButtonState state  = ButtonState::kPressed;
    //    bool ctrl       = false;
    //    bool alt        = false;
    //    bool shift      = false;

    //    KeyEvent() = default;
    //    virtual ~KeyEvent() override = default;

    //    MCP_DEFINE_EVENT_TYPE(kKey)

    //    // TODO: Define debug ostream overload.
    //};
}