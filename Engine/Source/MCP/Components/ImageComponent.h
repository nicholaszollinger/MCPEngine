#pragma once
// ImageComponent.h

#include "Component.h"
#include "../Graphics/Texture.h"
#include "MCP/Graphics/RenderData/BaseRenderData.h"
#include "MCP/Scene/IRenderable.h"
#include "Utility/Types/Color.h"

namespace mcp
{
    class TransformComponent;

    struct ImageComponentConstructionData
    {
        ComponentConstructionData componentData;
        const char* pImagePath = nullptr;
        RectInt crop = {};
        Color tint = Color::White();
        int zOrder = 0;
        bool sizedToContent = true;
    };

    class ImageComponent final : public Component, public IRenderable
    {
        MCP_DEFINE_COMPONENT_ID(ImageComponent)

        Texture m_texture;
        TransformComponent* m_pTransformComponent;
        RectInt m_crop {};
        Vec2 m_anglePivot;
        Vec2 m_scale;
        Color m_tint;
        double m_renderAngle;
        RenderFlip2D m_flip;
        bool m_sizedToContent;

    public:
        ImageComponent(const RenderLayer layer, const int zOrder);
        ImageComponent(const char* pTextureFilepath, const RectInt& crop = {}, const Vec2 scale = { 1.f, 1.f }, const Color tint = Color::White(), const RenderLayer layer = RenderLayer::kObject, const int zOrder = 0);
        explicit ImageComponent(const ImageComponentConstructionData& data);

        virtual bool Init() override;
        virtual void Render() const override;

        void SetTexture(const Texture* pTexture);
        void SetCrop(const RectInt& crop) { m_crop = crop; }
        void SetSize(const float width, const float height) { m_scale = { width, height }; }
        void SetTint(const Color color);
        void SetAlpha(const uint8_t alpha);

        [[nodiscard]] Color GetTint() const { return m_tint; }
        [[nodiscard]] RectInt GetCrop() const { return m_crop; }
        
        static ImageComponent* AddFromData(const XMLElement element);

    private:
        virtual void OnOwnerParentSet(Object* pParent) override;
        virtual void OnActive() override;
        virtual void OnInactive() override;
    };
}
