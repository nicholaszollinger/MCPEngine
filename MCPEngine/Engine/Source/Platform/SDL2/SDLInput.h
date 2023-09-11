#pragma once
// SDLInput

#include "MCP/Input/InputCodes.h"

class SDLInputImpl
{
public:
    static bool IsKeyPressed(const MCPKey& key);
    static bool IsKeyReleased(const MCPKey& key);
};