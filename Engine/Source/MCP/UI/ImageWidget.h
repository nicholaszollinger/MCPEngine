#pragma once
// ImageWidget.h

#include "Widget.h"
#include "MCP/Graphics/Texture.h"
#include "MCP/Graphics/RenderData/BaseRenderData.h"
#include "MCP/Scene/IRenderable.h"

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Just an image.
    //-----------------------------------------------------------------------------------------------------------------------------
    class ImageWidget final : public Widget, public IRenderable
    {
        MCP_DEFINE_WIDGET(ImageWidget)

        Texture m_texture;
        RectInt m_crop;
        Vec2 m_anglePivot;
        Color m_tint;
        double m_renderAngle = 0.0;
        RenderFlip2D m_flip = RenderFlip2D::kNone;

    public:
        ImageWidget(const WidgetConstructionData& data, const char* pImageFilePath);
        virtual ~ImageWidget() override;

        virtual bool Init() override;
        virtual void Render() const override;
        void SetTint(const Color tint) { m_tint = tint; }

        [[nodiscard]] Color GetTint() const { return m_tint; }
        virtual float GetRectWidth() const override;
        virtual float GetRectHeight() const override;

        static ImageWidget* AddFromData(const XMLElement element);
        static void RegisterLuaFunctions(lua_State* pState);
    private:
        virtual void OnActive() override;
        virtual void OnInactive() override;
        virtual void OnParentSet() override;
    };
}
