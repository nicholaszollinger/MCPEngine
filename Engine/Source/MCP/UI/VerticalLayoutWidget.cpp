// VerticalBox.cpp

#include "VerticalLayoutWidget.h"

#include "MCP/Scene/UILayer.h"

#if MCP_DEBUG_RENDER_LAYOUT_BOUNDS
#include "MCP/Graphics/Graphics.h"
#endif

namespace mcp
{
    VerticalLayoutWidget::VerticalLayoutWidget(const WidgetConstructionData& data, const LayoutData& layoutData)
        : Widget(data)
#if MCP_DEBUG_RENDER_LAYOUT_BOUNDS
        , IRenderable(RenderLayer::kDebugOverlay, 200)
#endif
        , m_horizontalAlignment(layoutData.horizontal)
        , m_verticalAlignment(layoutData.vertical)
        , m_padding(layoutData.padding)
    {
        m_sizedToContent = true;
    }

    VerticalLayoutWidget* VerticalLayoutWidget::AddFromData(const XMLElement element)
    {
        const auto data = GetWidgetConstructionData(element);
        const auto layoutData = GetLayoutData(element);

        return BLEACH_NEW(VerticalLayoutWidget(data, layoutData));
    }

    void VerticalLayoutWidget::OnChildAdded([[maybe_unused]] Widget* pChild)
    {
        Widget::OnChildAdded(pChild);
        RecalculateChildRects();
    }

    void VerticalLayoutWidget::OnChildRemoved([[maybe_unused]] Widget* pChild)
    {
        RecalculateChildRects();
    }

    void VerticalLayoutWidget::OnChildSizeChanged()
    {
        RecalculateChildRects();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : The width of a vertical layout widget is the max widget of its children.
    //-----------------------------------------------------------------------------------------------------------------------------
    float VerticalLayoutWidget::GetRectWidth() const
    {
        float maxWidth = 0.f;
        for (const auto* pChild : m_children)
        {
            const auto* pWidget = SafeCastEntity<Widget>(pChild);
            const auto childWidth = pWidget->GetRectWidth();
            if (maxWidth < childWidth)
            {
                maxWidth = childWidth;
            }
        }

        return maxWidth;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : The height of a vertical layout widget is equal to the total height of its immediate children plus the vertical
    ///         padding between them.
    //-----------------------------------------------------------------------------------------------------------------------------
    float VerticalLayoutWidget::GetRectHeight() const
    {
        float height = 0.f;
        for (const auto* pChild : m_children)
        {
            const auto* pWidget = SafeCastEntity<Widget>(pChild);
            height += pWidget->GetRectHeight();
        }

        height += m_padding * static_cast<float>(m_children.size() - 1);

        return height;
    }

    void VerticalLayoutWidget::RecalculateChildRects() const
    {
        if (m_children.empty())
            return;
        
        float lastPos = 0.f;

        for(auto* pChild : m_children)
        {
            auto* pWidget = SafeCastEntity<Widget>(pChild);
            SetChildAnchorAndPivot(pWidget);
            SetPositionAndMoveToNext(pWidget, lastPos);
        }

        if (m_pParent)
            GetParent()->OnChildSizeChanged();
    }

    void VerticalLayoutWidget::SetPositionAndMoveToNext(Widget* pChild, float& lastYPos) const
    {
        switch (m_verticalAlignment)
        {
            case VerticalAlignment::kCenter:
            {
                // Move down by half the child height
                const auto halfChildHeight = pChild->GetRectHeight() / 2.f;
                lastYPos += halfChildHeight;

                pChild->SetLocalPosition(0.f, lastYPos);

                // Move down by half to get to the end of the child.
                lastYPos += halfChildHeight;

                // Add the padding distance
                lastYPos += m_padding;

                break;
            }

            case VerticalAlignment::kTop:
            {
                pChild->SetLocalPosition(0.f, lastYPos);

                // Move down by the entire height of the child rect:
                lastYPos += pChild->GetRectHeight();

                // Add the padding distance
                lastYPos += m_padding;

                break;
            }

            case VerticalAlignment::kBottom:
            {
                pChild->SetLocalPosition(0.f, lastYPos);

                // Move up by the entire height of the child rect.
                lastYPos -= pChild->GetRectHeight();

                // Subtract the padding distance.
                lastYPos -= m_padding;

                break;
            }
        }
    }

    void VerticalLayoutWidget::SetChildAnchorAndPivot(Widget* pChild) const
    {
        // Default values are based on center alignment
        float xAnchorPivot = 0.5f;
        float yPivot = 0.5f;
        float yAnchor = 0.f;

        switch (m_verticalAlignment)
        {
            case VerticalAlignment::kCenter: break;
            case VerticalAlignment::kTop:
            {
                yPivot = 0.f;
                yAnchor = 0.f;
                break;
            }

            case VerticalAlignment::kBottom:
            {
                yPivot = 1.f;
                yAnchor = 1.f;
                break;
            }
        }

        switch (m_horizontalAlignment)
        {
            case HorizontalAlignment::kCenter: break;

            case HorizontalAlignment::kLeft:
            {
                xAnchorPivot = 0.f;
                break;
            }

            case HorizontalAlignment::kRight:
            {
                xAnchorPivot = 1.f;
                break;
            }
        }

        pChild->SetAnchor(xAnchorPivot, yAnchor);
        pChild->SetPivot(xAnchorPivot, yPivot);
    }

#if MCP_DEBUG_RENDER_LAYOUT_BOUNDS
    void VerticalLayoutWidget::OnActive()
    {
        MCP_LOG("VerticalLayout", "Rect: ", GetRect().ToString());
        m_isVisible = true;
        m_pUILayer->AddRenderable(this);
    }

    void VerticalLayoutWidget::OnInactive()
    {
        m_isVisible = false;
        m_pUILayer->RemoveRenderable(this);
    }

    void VerticalLayoutWidget::Render() const
    {
        DrawRect(GetRectTopLeft(), Color::Black()); 
    }
#endif


}
