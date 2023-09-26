#pragma once
// SDLHelpers.h

// ONLY INCLUDE THIS IN .CPP FILES.
// This is going to house any type conversions from SDL to my systems.

#pragma warning (push)
#pragma warning(disable : 26819)
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_scancode.h>
#pragma warning (pop)

#include "MCP/Graphics/RenderData/BaseRenderData.h"
#include "MCP/Input/InputCodes.h"
#include "Utility/Types/Rect.h"

struct Color;

namespace mcp
{
    // Keys
    MCPKey KeyToSDL(const SDL_Scancode scanCode);
    SDL_Scancode KeyToMCP(const MCPKey key);

    // Vec2:
    SDL_Point Vec2ToSdl(const Vec2Int& vec);
    SDL_FPoint Vec2ToSdlF(const Vec2& vec);

    // Rect:
    SDL_Rect RectToSdl(const RectInt& rect);
    SDL_FRect RectToSdlF(const RectF& rect);

    // Color:
    SDL_Color ColorToSdl(const Color& color);

    // RenderSettings:
    SDL_RendererFlip FlipToSdl(const mcp::RenderFlip2D& flip);

    SDL_Texture* CreateTextureFromSurface(SDL_Surface* pSurface, Vec2Int& sizeOut);
}