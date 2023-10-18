// VerticalBox.cpp

#include "VerticalLayoutWidget.h"

namespace mcp
{
    VerticalLayoutWidget::VerticalLayoutWidget(const WidgetConstructionData& data, const Justification justification)
        : Widget(data)
        , m_justification(justification)
    {
        //
    }

    VerticalLayoutWidget* VerticalLayoutWidget::AddFromData(const XMLElement element)
    {
        const auto data = GetWidgetConstructionData(element);

        const char* justificationLabel = element.GetAttributeValue<const char*>("justification", "center");
        const auto justification = static_cast<Justification>(HashString32(justificationLabel));

        return BLEACH_NEW(VerticalLayoutWidget(data, justification));
    }

    void VerticalLayoutWidget::OnChildAdded([[maybe_unused]] Widget* pChild)
    {
        RecalculateChildRects();
    }

    void VerticalLayoutWidget::OnChildRemoved([[maybe_unused]] Widget* pChild)
    {
        RecalculateChildRects();
    }

    void VerticalLayoutWidget::RecalculateChildRects()
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
            totalSize += pChild->GetRectHeight();

            if (m_justification == Justification::kLeft)
            {
                pChild->SetAnchor(0.0f, 0.0f);
            }

            else if (m_justification == Justification::kRight)
            {
                pChild->SetAnchor(1.f, 0.0f);
            }

            // Default to center.
            else
            {
                pChild->SetAnchor(0.5f, 0.0f);
            }
        }

        if (totalSize > m_height)
        {
            m_height = totalSize;
        }

        // TODO: I am going to assume this is larger for now.
        const float totalSpaceBetweenChildren = GetRectHeight() - totalSize;
        const float spaceBetweenChildren = totalSpaceBetweenChildren / static_cast<float>(m_children.size() - 1);

        const auto rectWidth = GetRectWidth();

        float lastPos = 0.f;
        for (size_t i = 0; i < m_children.size(); ++i)
        {
            auto* pChild = m_children[i];

            // Start from our last position.
            float offset = lastPos;

            // If this isn't the first item, move over by the space between.
            if (i != 0)
                offset += spaceBetweenChildren;

            const auto halfChildHeight = (pChild->GetRectHeight() / 2.f);

            // Move over half the width, to center it.
            offset += halfChildHeight;

            float localXPos = 0.f;

            if (m_justification == Justification::kLeft)
            {
                localXPos = pChild->GetRectWidth() / 2.f;
            }

            else if (m_justification == Justification::kRight)
            {
                localXPos = -(pChild->GetRectWidth() / 2.f);
            }

            pChild->SetLocalPosition(localXPos, offset);

            // Update the last position.
            lastPos = offset + halfChildHeight;
        }
    }
}
