// SDLGraphics.cpp

#include "SDLRenderer.h"

#pragma warning(push)
#pragma warning(disable : 26819)
#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#pragma warning(pop)

#include <cassert>
#include "MCP/Core/Application/Window/WindowBase.h"
#include "MCP/Debug/Log.h"
#include "MCP/Graphics/Graphics.h"
#include "Platform/SDL2/SDLHelpers.h"

bool SdlRenderer::Init()
{
    if (SDL_VideoInit(nullptr) != 0)
    {
        MCP_ERROR("SDL", "Failed to initialize SDL_Video! SDL_Error: ", SDL_GetError());
        return false;
    }

    constexpr int kFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (IMG_Init(kFlags) != kFlags)
    {
        MCP_ERROR("SDL", "Failed to initialize SDL_Image! IMG_Error: ", IMG_GetError());
        return false;
    }

    // Initialize the sdl fonts.
    if (TTF_Init() != 0)
    {
        MCP_ERROR("SDL", "Failed to initialize SDL_ttf! TTF_Error: ", TTF_GetError());
    }

    return true;
}

bool SdlRenderer::SetRenderTarget(mcp::WindowBase* pWindow)
{
    assert(pWindow);

    s_pWindow = pWindow;
    s_pRenderer = static_cast<SDL_Renderer*>(s_pWindow->GetRenderer());
    if (!s_pRenderer)
    {
        MCP_ERROR("SDLRenderer", "Failed to get SDL_Renderer from mcp::WindowBase*!");
        return false;
    }

    return true;
}

void SdlRenderer::Display()
{
    SDL_RenderPresent(s_pRenderer);
}

void SdlRenderer::Close()
{
    TTF_Quit();
    IMG_Quit();
    SDL_VideoQuit();
}

void SdlRenderer::SetDrawColor(const Color& color)
{
    if(SDL_SetRenderDrawColor(s_pRenderer, color.r, color.g, color.b, color.alpha) < 0)
    {
        MCP_ERROR("SDL", "Failed to set color! SDL_Error: ", SDL_GetError());
    }
}

void SdlRenderer::FillScreen(const Color& color)
{
    SetDrawColor(color);
    if (SDL_RenderClear(s_pRenderer) != 0)
    {
        MCP_ERROR("SDL", "Failed to clear screen! SDL_Error: ", SDL_GetError());
    }
}

void SdlRenderer::DrawLine(const Vec2Int& a, const Vec2Int& b, const Color& color)
{
    SetDrawColor(color);
    if (SDL_RenderDrawLine(s_pRenderer, a.x, a.y, b.x, b.y) != 0)
    {
        MCP_ERROR("SDL", "Failed to draw line! SDL_Error: ", SDL_GetError());
    }
}

void SdlRenderer::DrawFillRect(const RectInt& rect, const Color& color)
{
    SetDrawColor(color);
    const auto sdlRect = mcp::RectToSdl(rect);

    if (SDL_RenderFillRect(s_pRenderer, &sdlRect) != 0)
    {
        MCP_ERROR("SDL", "Failed to draw fill rect! SDL_Error: ", SDL_GetError());
    }
}

void SdlRenderer::DrawFillRect(const RectF& rect, const Color& color)
{
    SetDrawColor(color);
    const auto sdlRect = mcp::RectToSdlF(rect);

    if (SDL_RenderFillRectF(s_pRenderer, &sdlRect) != 0)
    {
        MCP_ERROR("SDL", "Failed to draw fill rect! SDL_Error: ", SDL_GetError());
    }
}

void SdlRenderer::DrawRect(const RectInt& rect, const Color& color)
{
    SetDrawColor(color);
    const auto sdlRect = mcp::RectToSdl(rect);

    if (SDL_RenderDrawRect(s_pRenderer, &sdlRect) != 0)
    {
        MCP_ERROR("SDL", "Failed to draw rect! SDL_Error: ", SDL_GetError());
    }
}

void SdlRenderer::DrawRect(const RectF& rect, const Color& color)
{
    SetDrawColor(color);
    const auto sdlRect = mcp::RectToSdlF(rect);

    if (SDL_RenderDrawRectF(s_pRenderer, &sdlRect) != 0)
    {
        MCP_ERROR("SDL", "Failed to draw rect! SDL_Error: ", SDL_GetError());
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      Here is my reference. It is an example of the 'Midpoint Circle Algorithm'.
//      https://stackoverflow.com/questions/38334081/howto-draw-circles-arcs-and-vector-graphics-in-sdl
//
///		@brief : Draw a circle to the screen.
//-----------------------------------------------------------------------------------------------------------------------------
void SdlRenderer::DrawCircle(const Vec2& pos, const float radius, const Color& color)
{
    int errorCode = 0;
    SetDrawColor(color);

    const float diameter = radius * 2.f;
    float x = radius - 1.f;
    float y = 0.f;
    float tx = 1.f;
    float ty = 1.f;
    float error = (tx - diameter);

    while (x >= y)
    {
        // Each of the following renders an octant of the circle
        errorCode = SDL_RenderDrawPointF(s_pRenderer, pos.x + x, pos.y - y);
        errorCode = SDL_RenderDrawPointF(s_pRenderer, pos.x + x, pos.y + y);
        errorCode = SDL_RenderDrawPointF(s_pRenderer, pos.x - x, pos.y - y);
        errorCode = SDL_RenderDrawPointF(s_pRenderer, pos.x - x, pos.y + y);
        errorCode = SDL_RenderDrawPointF(s_pRenderer, pos.x + y, pos.y - x);
        errorCode = SDL_RenderDrawPointF(s_pRenderer, pos.x + y, pos.y + x);
        errorCode = SDL_RenderDrawPointF(s_pRenderer, pos.x - y, pos.y - x);
        errorCode = SDL_RenderDrawPointF(s_pRenderer, pos.x - y, pos.y + x);

        if (error <= 0.f)
        {
            ++y;
            error += ty;
            ty += 2.f;
        }

        if (error > 0.f)
        {
            --x;
            tx += 2.f;
            error += (tx - diameter);
        }
    }
}

void SdlRenderer::DrawTexture(const mcp::TextureRenderData& context)
{
    const SDL_Rect crop = mcp::RectToSdl(context.crop);
    const SDL_FRect dst = mcp::RectToSdlF(context.destinationRect);
    const SDL_FPoint center = mcp::Vec2ToSdlF(context.anglePivot);
    auto* pSdlTexture = static_cast<SDL_Texture*>(context.pTexture);

    // Set the new tint color
    if (SDL_SetTextureColorMod(pSdlTexture, context.tint.r, context.tint.g, context.tint.b) != 0)
    {
        MCP_ERROR("SDL", "Failed to set SDL_Texture Color! SDL_Error: ", SDL_GetError());
    }

    // Set the new alpha value.
    if (SDL_SetTextureAlphaMod(pSdlTexture, context.tint.alpha) != 0)
    {
        MCP_ERROR("SDL", "Failed to set SDL Alpha Alpha! SDL_Error: ", SDL_GetError());
    }

    if (SDL_RenderCopyExF(
        s_pRenderer
        , pSdlTexture
        , &crop
        , &dst
        , context.angle
        , &center
        , mcp::FlipToSdl(context.flip)
    ) != 0)
    {
        MCP_ERROR("SDL", "Failed to draw texture! SDL_Error: ", SDL_GetError());
    }
}