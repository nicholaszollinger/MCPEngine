#pragma once
// Rect2DComponent.h

#include "PrimitiveComponent.h"

namespace mcp
{
    class Rect2DComponent final : public PrimitiveComponent
    {
        MCP_DEFINE_COMPONENT_ID(Rect2DComponent)
    //public:
    //    static inline const mcp::ComponentTypeId kComponentTypeId = mcp::ComponentFactory::RegisterComponentType<Rect2DComponent>("Rect2DComponent");
    //    virtual mcp::ComponentTypeId GetTypeId() const override { return kComponentTypeId; }
    private:
        float m_width;
        float m_height;

    public:
        Rect2DComponent(Object* pObject, const float width, const float height, const RenderLayer = RenderLayer::kObject, const int zOrder = 0);

        virtual void Render() const override;

        void SetWidth(const float width) { m_width = width; }
        void SetHeight(const float height) { m_height = height; }
        void SetDimensions(const float width, const float height) { m_width = width; m_height = height; }

        static bool AddFromData(const void* pFileData, Object* pOwner);
    };
}
