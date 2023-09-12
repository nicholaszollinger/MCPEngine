// SDLHelpers.cpp

#include "SDLHelpers.h"

#include "Utility/Types/Color.h"

namespace mcp
{
    MCPKey KeyToSDL(const SDL_Scancode scanCode)
    {
        switch (scanCode)
        {
            // Letters
            case SDL_SCANCODE_A: return MCPKey::A;
            case SDL_SCANCODE_B: return MCPKey::B;
            case SDL_SCANCODE_C: return MCPKey::C;
            case SDL_SCANCODE_D: return MCPKey::D;
            case SDL_SCANCODE_E: return MCPKey::E;
            case SDL_SCANCODE_F: return MCPKey::F;
            case SDL_SCANCODE_G: return MCPKey::G;
            case SDL_SCANCODE_H: return MCPKey::H;
            case SDL_SCANCODE_I: return MCPKey::I;
            case SDL_SCANCODE_J: return MCPKey::J;
            case SDL_SCANCODE_K: return MCPKey::K;
            case SDL_SCANCODE_L: return MCPKey::L;
            case SDL_SCANCODE_M: return MCPKey::M;
            case SDL_SCANCODE_N: return MCPKey::N;
            case SDL_SCANCODE_O: return MCPKey::O;
            case SDL_SCANCODE_P: return MCPKey::P;
            case SDL_SCANCODE_Q: return MCPKey::Q;
            case SDL_SCANCODE_R: return MCPKey::R;
            case SDL_SCANCODE_S: return MCPKey::S;
            case SDL_SCANCODE_T: return MCPKey::T;
            case SDL_SCANCODE_U: return MCPKey::U;
            case SDL_SCANCODE_V: return MCPKey::V;
            case SDL_SCANCODE_W: return MCPKey::W;
            case SDL_SCANCODE_X: return MCPKey::X;
            case SDL_SCANCODE_Y: return MCPKey::Y;
            case SDL_SCANCODE_Z: return MCPKey::Z;

            // Numbers
            // ...

            // Special
            case SDL_SCANCODE_ESCAPE: return MCPKey::Escape;
            case SDL_SCANCODE_SPACE: return MCPKey::Space;
            case SDL_SCANCODE_RETURN:
            case SDL_SCANCODE_RETURN2: return MCPKey::Enter;

        default:
            return MCPKey::kInvalid;
        }
    }

    SDL_Scancode KeyToMCP(const MCPKey key)
    {
        switch (key)
        {
            // Letters
            case MCPKey::A: return SDL_SCANCODE_A;
            case MCPKey::B: return SDL_SCANCODE_B;
            case MCPKey::C: return SDL_SCANCODE_C;
            case MCPKey::D: return SDL_SCANCODE_D;
            case MCPKey::E: return SDL_SCANCODE_E;
            case MCPKey::F: return SDL_SCANCODE_F;
            case MCPKey::G: return SDL_SCANCODE_G;
            case MCPKey::H: return SDL_SCANCODE_H;
            case MCPKey::I: return SDL_SCANCODE_I;
            case MCPKey::J: return SDL_SCANCODE_J;
            case MCPKey::K: return SDL_SCANCODE_K;
            case MCPKey::L: return SDL_SCANCODE_L;
            case MCPKey::M: return SDL_SCANCODE_M;
            case MCPKey::N: return SDL_SCANCODE_N;
            case MCPKey::O: return SDL_SCANCODE_O;
            case MCPKey::P: return SDL_SCANCODE_P;
            case MCPKey::Q: return SDL_SCANCODE_Q;
            case MCPKey::R: return SDL_SCANCODE_R;
            case MCPKey::S: return SDL_SCANCODE_S;
            case MCPKey::T: return SDL_SCANCODE_T;
            case MCPKey::U: return SDL_SCANCODE_U;
            case MCPKey::V: return SDL_SCANCODE_V;
            case MCPKey::W: return SDL_SCANCODE_W;
            case MCPKey::X: return SDL_SCANCODE_X;
            case MCPKey::Y: return SDL_SCANCODE_Y;
            case MCPKey::Z: return SDL_SCANCODE_Z;

            // Numbers
            // ...

            // Special
            case MCPKey::Escape:   return SDL_SCANCODE_ESCAPE;
            case MCPKey::Space:    return  SDL_SCANCODE_SPACE;
            case MCPKey::Enter:    return SDL_SCANCODE_RETURN;
                
        default:
                return SDL_SCANCODE_UNKNOWN;
        }
    }

    SDL_Rect RectToSdl(const RectInt& rect)
    {
        return SDL_Rect{ rect.x, rect.y, rect.width, rect.height };
    }

    SDL_FRect RectToSdlF(const RectF& rect)
    {
        return SDL_FRect{ rect.x, rect.y, rect.width, rect.height };
    }

    SDL_Color ColorToSdl(const Color& color)
    {
        return SDL_Color{ color.r, color.g, color.b, color.alpha };
    }

    SDL_RendererFlip FlipToSdl(const mcp::RenderFlip2D& flip)
    {
        switch (flip)
        {
            case RenderFlip2D::kNone: return SDL_FLIP_NONE;
            case RenderFlip2D::kHorizontal: return SDL_FLIP_HORIZONTAL;
            case RenderFlip2D::kVertical: return SDL_FLIP_VERTICAL;
        }

        return SDL_FLIP_NONE;
    }

    SDL_Point Vec2ToSdl(const Vec2Int& vec)
    {
        return { vec.x, vec.y };
    }

    SDL_FPoint Vec2ToSdlF(const Vec2& vec)
    {
        return { vec.x, vec.y };
    }
}