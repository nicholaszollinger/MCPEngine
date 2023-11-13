// HorizontalBox.cpp

#include "HorizontalLayoutWidget.h"

#include "MCP/Scene/UILayer.h"

#if MCP_DEBUG_RENDER_LAYOUT_BOUNDS
#include "MCP/Graphics/Graphics.h"
#endif

namespace mcp
{
    HorizontalLayoutWidget::HorizontalLayoutWidget(const WidgetConstructionData& data, const LayoutData& layoutData)
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

    HorizontalLayoutWidget* HorizontalLayoutWidget::AddFromData(const XMLElement element)
    {
        const auto data = GetWidgetConstructionData(element);

        const auto layoutData = GetLayoutData(element);

        return BLEACH_NEW(HorizontalLayoutWidget(data, layoutData));
    }

    void HorizontalLayoutWidget::OnChildAdded([[maybe_unused]] Widget* pChild)
    {
        Widget::OnChildAdded(pChild);
        RecalculateChildRects();
    }

    void HorizontalLayoutWidget::OnChildRemoved([[maybe_unused]] Widget* pChild)
    {
        RecalculateChildRects();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : The total width of a Horizontal Widget is equal to the total width of its immediate children plus the horizontal
    ///         padding between them.
    //-----------------------------------------------------------------------------------------------------------------------------
    float HorizontalLayoutWidget::GetRectWidth() const
    {
        float totalWidth = 0.f;
        for (const auto* pChild : m_children)
        {
            const auto* pWidget = SafeCastEntity<Widget>(pChild);
            totalWidth += pWidget->GetRectWidth();
        }

        totalWidth += m_padding * static_cast<float>(m_children.size() - 1);

        return totalWidth;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : The height of a horizontal widget is the max height of its children.
    //-----------------------------------------------------------------------------------------------------------------------------
    float HorizontalLayoutWidget::GetRectHeight() const
    {
        float maxHeight = 0.f;
        for (const auto* pChild : m_children)
        {
            const auto* pWidget = SafeCastEntity<Widget>(pChild);

            const auto childHeight = pWidget->GetRectHeight();
            if (maxHeight < childHeight)
            {
                maxHeight = childHeight;
            }
        }

        return maxHeight;
    }

    void HorizontalLayoutWidget::OnChildSizeChanged()
    {
        RecalculateChildRects();
    }

    void HorizontalLayoutWidget::RecalculateChildRects() const
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

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the local position of the Child widget and move the 'lastXPos' to be at the beginning of the next child.
    //-----------------------------------------------------------------------------------------------------------------------------
    void HorizontalLayoutWidget::SetPositionAndMoveToNext(Widget* pChild, float& lastXPos) const
    {
        float localYPos = 0.f;

        // Update the lastXPos position based on the Horizontal arrangement.
        switch (m_horizontalAlignment)
        {
            case HorizontalAlignment::kCenter:
            {
                // Increment by half the width.
                const auto halfChildWidth = pChild->GetRectWidth() / 2.f;
                lastXPos += halfChildWidth;

                pChild->SetLocalPosition(lastXPos, localYPos);

                // Move over by half width to get the the end of the child.
                lastXPos += halfChildWidth;

                // Add the padding distance
                lastXPos += m_padding;

                break;
            }

            case HorizontalAlignment::kLeft:
            {
                pChild->SetLocalPosition(lastXPos, localYPos);

                // Move over by the entire width of the child rect.
                lastXPos += pChild->GetRectWidth();

                // Add the padding distance
                lastXPos += m_padding;

                break;
            }

            case HorizontalAlignment::kRight:
            {
                pChild->SetLocalPosition(lastXPos, localYPos);

                // Move back by the entire width of the child rect.
                lastXPos -= pChild->GetRectWidth();

                // Subtract the padding distance
                lastXPos -= m_padding;

                break;
            }
        }
    }

    void HorizontalLayoutWidget::SetChildAnchorAndPivot(Widget* pChild) const
    {
        // Default values are based on Center alignment.
        float yAnchorPivot = 0.5f;
        float xPivot = 0.5f;
        float xAnchor = 0.f;

        switch (m_verticalAlignment)
        {
            case VerticalAlignment::kCenter: break;
            case VerticalAlignment::kTop:
            {
                yAnchorPivot = 0.f;
                break;
            }

            case VerticalAlignment::kBottom:
            {
                yAnchorPivot = 1.f;
                break;
            }
        }

        switch (m_horizontalAlignment)
        {
            case HorizontalAlignment::kCenter: break;

            case HorizontalAlignment::kLeft:
            {
                xAnchor = 0.f;
                xPivot = 0.f;
                break;
            }

            case HorizontalAlignment::kRight:
            {
                xAnchor = 1.f;
                xPivot = 1.f;
                break;
            }
        }

        pChild->SetAnchor(xAnchor, yAnchorPivot);
        pChild->SetPivot(xPivot, yAnchorPivot);
    }

#if MCP_DEBUG_RENDER_LAYOUT_BOUNDS
    void HorizontalLayoutWidget::OnActive()
    {
        MCP_LOG("HorizontalLayout", "Rect: ", GetRect().ToString());
        m_isVisible = true;
        m_pUILayer->AddRenderable(this);
    }

    void HorizontalLayoutWidget::OnInactive()
    {
        m_isVisible = false;
        m_pUILayer->RemoveRenderable(this);
    }

    void HorizontalLayoutWidget::Render() const
    {
        DrawRect(GetRectTopLeft(), Color::Black()); 
    }
#endif

}