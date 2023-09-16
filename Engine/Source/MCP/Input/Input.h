#pragma once
// Input.h

#include "MCP/Input/InputCodes.h"

namespace mcp
{
    bool IsKeyPressed(const MCPKey& key);
    bool IsKeyReleased(const MCPKey& key);

}