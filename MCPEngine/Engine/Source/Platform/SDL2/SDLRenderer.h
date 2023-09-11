#pragma once
// SDLGraphics.h

#include "utility/Rect.h"

struct SDL_Renderer;
struct Color;
namespace mcp
{
    class WindowBase;
    struct TextureRenderData;
}

class SdlRenderer
{
    inline static mcp::WindowBase* s_pWindow = nullptr;
    inline static SDL_Renderer* s_pRenderer = nullptr;

public:
    static bool Init();
    static bool SetRenderTarget(mcp::WindowBase* pWindow);
    static void Display();
    static void Close();
    static void SetDrawColor(const Color& color);
    static void FillScreen(const Color& color);

    static void DrawLine(const Vec2Int& a, const Vec2Int& b, const Color& color);
    static void DrawFillRect(const RectInt& rect, const Color& color);
    static void DrawFillRect(const RectF& rect, const Color& color);
    static void DrawRect(const RectInt& rect, const Color& color);
    static void DrawRect(const RectF& rect, const Color& color);
    static void DrawCircle(const Vec2& pos, const float radius, const Color& color);
    static void DrawTexture(const mcp::TextureRenderData& context);
};
