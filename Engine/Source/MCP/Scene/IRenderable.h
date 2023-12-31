#pragma once
// IRenderable.h
#include <cstdint>

namespace mcp
{
    using RenderableId = uint32_t;

    enum class RenderLayer
    {
        kWorld,         // The 'background' essentially.
        kObject,        // Where scene objects live.
        kDebugOverlay,  // Layer for debugging overlays, like collider bounds, etc.
    };

    class IRenderable
    {
        static inline RenderableId s_idCounter = 0;
        
        const RenderableId m_renderableId;
        int m_zOrder;
        RenderLayer m_renderLayer;

    public:
        IRenderable(const RenderLayer layer, const int zOrder = 0);
        virtual ~IRenderable() = default;
        virtual void Render() const = 0;

        [[nodiscard]] RenderableId GetRenderId() const { return m_renderableId;}
        [[nodiscard]] RenderLayer GetRenderLayer() const { return m_renderLayer;}
        [[nodiscard]] int GetZOrder() const { return m_zOrder;}
        void SetZOrder(const int zOrder) { m_zOrder = zOrder; }
    };
}
