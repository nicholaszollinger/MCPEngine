// Input.cpp

#include "Input.h"

#include "MCP/Core/Config.h"

#if MCP_INPUT_PLATFORM == MCP_INPUT_PLATFORM_SDL
#include "Platform/SDL2/SDLInput.h"

namespace mcp
{
    using InputStateImplementation = SDLInputImpl;
}

#else
#error "We don't have a InputState Implementation set up for that platform!'"
#endif

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Returns true if the key is currently down, false otherwise.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Input::IsKeyPressed(const MCPKey& key)
    {
        return InputStateImplementation::IsKeyPressed(key);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Returns whether the key is *not* down.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Input::IsKeyReleased(const MCPKey& key)
    {
        return InputStateImplementation::IsKeyReleased(key);
    }
}