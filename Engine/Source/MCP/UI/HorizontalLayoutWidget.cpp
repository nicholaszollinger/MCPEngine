// HorizontalBox.cpp

#include "HorizontalLayoutWidget.h"

namespace mcp
{
    HorizontalLayoutWidget::HorizontalLayoutWidget(const WidgetConstructionData& data)
        : Widget(data)
    {
        //        
    }

    HorizontalLayoutWidget* HorizontalLayoutWidget::AddFromData(const XMLElement element)
    {
        const auto data = GetWidgetConstructionData(element);
        return BLEACH_NEW(HorizontalLayoutWidget(data));
    }

    void HorizontalLayoutWidget::OnChildAdded([[maybe_unused]] Widget* pChild)
    {
        RecalculateChildRects();
    }

    void HorizontalLayoutWidget::OnChildRemoved([[maybe_unused]] Widget* pChild)
    {
        RecalculateChildRects();
    }

    void HorizontalLayoutWidget::RecalculateChildRects()
    {
        // TODO: I am only centering. I need other alignments.
        if (m_children.empty())
            return;

        if (m_children.size() == 1)
        {
            // Set the only child as the center of our box.
            m_children[0]->SetLocalPosition(0.f, 0.f);
            m_children[0]->SetAnchor(0.5f, 0.5f);
            return;
        }

        // Calculate the total size of the children
        float totalSize = 0.f;
        for (auto* pChild : m_children)
        {
            totalSize += pChild->GetRectWidth();
            pChild->SetAnchor(0.f, 0.5f);
        }

        if (totalSize > m_width)
        {
            m_width = totalSize;
        }

        // TODO: I am going to assume this is larger for now.
        const float totalSpaceBetweenChildren = GetRectWidth() - totalSize;
        const float spaceBetweenChildren = totalSpaceBetweenChildren / static_cast<float>(m_children.size() - 1);

        float lastPos = 0.f;
        for (size_t i = 0; i < m_children.size(); ++i)
        {
            auto* pChild = m_children[i];

            // Start from our last position.
            float offset = lastPos;

            // If this isn't the first item, move over by the space between.
            if (i != 0)
                offset += spaceBetweenChildren;

            // Move over half the width, to center it.
            offset += (pChild->GetRectWidth() / 2.f);
            pChild->SetLocalPosition(offset, 0.f);

            // Update the last position.
            lastPos = offset + (pChild->GetRectWidth() / 2.f);
        }
    }
}