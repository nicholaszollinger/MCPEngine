#pragma once
// Input.h

#include "MCP/Input/InputCodes.h"

namespace mcp
{
    class Input
    {
    public:
        static bool IsKeyPressed(const MCPKey& key);
        static bool IsKeyReleased(const MCPKey& key);
    };

}