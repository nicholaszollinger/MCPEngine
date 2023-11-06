// InputCodes.cpp

#include "InputCodes.h"

namespace mcp
{
    bool IsAlpha(const MCPKey key)
    {
        return key >= MCPKey::A && key <= MCPKey::Z;
    }


    bool IsNumeric(const MCPKey key)
    {
        return key >= MCPKey::Num0 && key <= MCPKey::Num9;
    }

    bool IsAlphaNumeric(const MCPKey key)
    {
        return IsAlpha(key) || IsNumeric(key);
    }
} 