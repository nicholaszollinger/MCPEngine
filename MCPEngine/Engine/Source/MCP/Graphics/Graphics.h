#pragma once
// Graphics.h

#include "MCP/Core/GlobalManager.h"
#include "MCP/Core/Application/Window/WindowBase.h"
#include "RenderData/BaseRenderData.h"
#include "utility/Color.h"

namespace mcp
{
    struct TextureRenderData final : public BaseRenderData
    {
        void* pTexture = nullptr;
    };

    class GraphicsManager final : public IProcess
    {
        DEFINE_GLOBAL_MANAGER(GraphicsManager)
        WindowBase* m_pWindow = nullptr;

    public:
        [[nodiscard]] WindowBase* GetWindow() const { return m_pWindow; }
        [[nodiscard]] void* GetRenderer() const;

    private:
        virtual bool Init() override;
        virtual void Close() override;
        [[nodiscard]] bool SetRenderTarget() const;
        void Display();
    };

    void SetDrawColor(const Color& color);
    void FillScreen(const Color& color);

    void DrawLine(const Vec2Int& a, const Vec2Int& b, const Color& color);
    void DrawFillRect(const RectInt& rect, const Color& color);
    void DrawFillRect(const RectF& rect, const Color& color);
    void DrawRect(const RectInt& rect, const Color& color);
    void DrawRect(const RectF& rect, const Color& color);
    void DrawCircle(const Vec2& pos, const float radius, const Color& color);
    void DrawTexture(const TextureRenderData& context);
}