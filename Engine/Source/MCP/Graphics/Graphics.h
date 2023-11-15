#pragma once
// Graphics.h

#include "MCP/Core/Application/Window/WindowBase.h"
#include "RenderData/BaseRenderData.h"
#include "Utility/Types/Color.h"
#include "MCP/Core/System.h"

namespace mcp
{
    struct TextureRenderData final : public BaseRenderData
    {
        void* pTexture = nullptr;
        Color tint = {255,255,255, 255};   // Tint color including alpha value.
    };

    struct WindowConstructionData
    {
        std::string windowName = "Game";
        Vec2Int dimensions = {1600, 900};
    };

    class GraphicsManager final : public System
    {
        MCP_DEFINE_SYSTEM(GraphicsManager)

        WindowConstructionData m_mainWindowData;
        WindowBase* m_pWindow = nullptr;

        GraphicsManager(WindowConstructionData&& data);

    public:
        static GraphicsManager* Get();
        [[nodiscard]] WindowBase* GetWindow() const { return m_pWindow; }
        [[nodiscard]] void* GetRenderer() const;

        static GraphicsManager* AddFromData(const XMLElement element);
    private:
        virtual bool Init() override;
        virtual void Close() override;
        [[nodiscard]] bool SetRenderTarget(WindowBase* pTarget) const;
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