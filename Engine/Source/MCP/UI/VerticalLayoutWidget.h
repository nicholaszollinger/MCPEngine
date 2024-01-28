#pragma once
// VerticalLayoutWidget.h
#include "Layout.h"
#include "Widget.h"

#if MCP_DEBUG_RENDER_LAYOUT_BOUNDS
#include "MCP/Scene/IRenderable.h"
#endif

namespace mcp
{
    class VerticalLayoutWidget final : public Widget
#if MCP_DEBUG_RENDER_LAYOUT_BOUNDS
        , public IRenderable
#endif
    {
        MCP_DEFINE_WIDGET(VerticalLayoutWidget)

        HorizontalAlignment m_horizontalAlignment;
        VerticalAlignment m_verticalAlignment;
        float m_padding;                        // Distance between each child element.

    public:
        VerticalLayoutWidget(const WidgetConstructionData& data, const LayoutData& layoutData);

        [[nodiscard]]virtual float GetRectWidth() const override;
        [[nodiscard]]virtual float GetRectHeight() const override;

        static VerticalLayoutWidget* AddFromData(const XMLElement element);

    private:
        virtual void OnChildAdded(Widget* pChild) override;
        virtual void OnChildRemoved(Widget* pChild) override;
        virtual void OnChildSizeChanged() override;

        void SetPositionAndMoveToNext(Widget* pChild, float& lastYPos) const;
        void SetChildAnchorAndPivot(Widget* pChild) const;
        void RecalculateChildRects() const;

#if MCP_DEBUG_RENDER_LAYOUT_BOUNDS
    public:
        virtual void OnActive() override;
        virtual void OnInactive() override;
        virtual void Render() const override;
    private:
#endif
    };
}