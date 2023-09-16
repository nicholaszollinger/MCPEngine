// SDLInput.cpp

#include "SDLInput.h"

#pragma warning(push)
#pragma warning(disable : 26819)
#include <SDL_keyboard.h>
#pragma warning (pop)

#include "SDLHelpers.h"

bool SDLInputImpl::IsKeyPressed(const MCPKey& key)
{
    const Uint8* pKeyStates = SDL_GetKeyboardState(nullptr);
    if (pKeyStates[mcp::KeyToMCP(key)])
    {
        return true;
    }

    return false;
}

bool SDLInputImpl::IsKeyReleased(const MCPKey& key)
{
    const Uint8* pKeyStates = SDL_GetKeyboardState(nullptr);
    if (!pKeyStates[mcp::KeyToMCP(key)])
    {
        return true;
    }

    return false;
}
