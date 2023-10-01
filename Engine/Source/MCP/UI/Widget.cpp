// Widget.cpp

#include "Widget.h"

#include "MCP/Core/Event/ApplicationEvent.h"
#include "MCP/Debug/Assert.h"
#include "MCP/Debug/Log.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    Widget::Widget(const WidgetConstructionData& data)
        : m_pUILayer(nullptr)
        , m_pParent(nullptr)
        , m_offset(data.rect.GetPosition())
        , m_anchor(data.anchor)
        , m_width(data.rect.width)
        , m_height(data.rect.height)
        , m_zOffset(0)
        , m_isInteractable(data.isInteractable)
        , m_isActive(true)
    {
        SetZOffset(data.zOffset);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the parent of this widget.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetParent(Widget* pParent)
    {
        // If this is a new parent,
        if (pParent != m_pParent)
        {
            // If our parent was not nullptr, then we need to remove ourselves as the child of our parent.
            if (m_pParent)
            {
                m_pParent->RemoveChild(this);
            }

            // If our new parent isn't nullptr, we need to add ourselves as a child.
            // This will set the parent relationship.
            if (pParent)
            {
                pParent->AddChild(this);
            }
        }
    }

    void Widget::AddChild(Widget* pChild)
    {
        m_children.push_back(pChild);
        pChild->m_pParent = this;
    }

    void Widget::RemoveChild(Widget* pChild)
    {
        for (size_t i = 0; i < m_children.size(); ++i)
        {
            if (pChild == m_children[i])
            {
                std::swap(m_children[i], m_children.back());
                m_children.pop_back();
                return;
            }
        }

        MCP_WARN("Widget", "Tried to remove a child that doesn't not exist on the parent!");
    }


    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Queues the destruction of this widget. All children of this widget will be linked to the parent of this child.
    ///         If this Widget is the root, this function will do nothing. Deleting the Root must be done with DestroyWidgetBranch().
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::DestroyWidget()
    {
        // If we are the root, we do nothing.
        if (!m_pParent)
            return;

        // Set each child's parent to be this Widget's parent, and add the child to that parent's children.
        for (auto* pChild : m_children)
        {
            pChild->SetParent(m_pParent);
            m_pParent->AddChild(pChild);
        }

        // Now that we are outside of the tree, queue this Widget for deletion.
        m_pUILayer->DeleteWidget(this);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Queues the destruction of this widget and all of its Children. If this widget is the Root, then the entire
    ///         tree will be destroyed.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::DestroyWidgetAndChildren()
    {
        // Queue the Destruction of each child.
        for (auto* pChild : m_children)
        {
            pChild->DestroyWidgetAndChildren();
        }

        // Now that all of the children have been queued for deletion,
        // Queue ourselves for deletion.
        m_pUILayer->DeleteWidget(this);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Focuses this Widget. A Focused widget and all of it's children will be recipients of input events.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::Focus()
    {
        m_pUILayer->FocusWidget(this);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Sends the event down the stack to see if anyone handles it.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnEvent(ApplicationEvent& event)
    {
        // If we have children, we need to check to see if they should handle the event
        // 'Move down the tree'
        if (HasChildren())
        {
            for (auto* pChild : m_children)
            {
                pChild->OnEvent(event);
                if (event.IsHandled())
                    return;
            }
        }

        // If the event was not handled by any of our children and we are interactable,
        // then handle the event.
        if (!event.IsHandled() && m_isInteractable)
        {
            HandleEvent(event);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the positional offset from the anchor position. If this widget has no parent, then this will effectively be
    ///         its position on the screen.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetPositionOffset(const float x, const float y)
    {
        m_offset.x = x;
        m_offset.y = y;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : The Anchor represents the positional relationship to a parent Widget. Both x and y are values in the range [0,1].
    ///         If x == 0, then this Widget's position is offset from the left side of the parent rect. If x == 1, its position is
    ///         offset from the right side of the parent rect. Same goes for the y value. If you want to anchor it to the center
    ///         of the parent, you would want to pass in (0.5f, 0.5f).
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetAnchor(const float x, const float y)
    {
        m_anchor.x = std::clamp(x, 0.f, 1.f);
        m_anchor.y = std::clamp(y, 0.f, 1.f);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns a RectF that describes where the Widget is on the screen. The x and y position of the RectF represent
    ///             the center of the Rect.
    //-----------------------------------------------------------------------------------------------------------------------------
    RectF Widget::GetScreenRect() const
    {
        // If we are the root, then just return our rect.
        if (m_pParent == nullptr)
            return RectF(m_offset.x, m_offset.y, m_width, m_height);

        // Otherwise, get the rect of the Parent, and return our rect based on our anchor and offset.
        const auto parentRect = m_pParent->GetScreenRect();
        const auto startPos = Vec2( parentRect.x + (m_anchor.x - 0.5f) * parentRect.width,  parentRect.y + (m_anchor.y - 0.5f) * parentRect.height);
        const auto offsetPos = startPos + m_offset;
        //Vec2(m_width / 2.f, m_height / 2.f);

        return RectF(offsetPos.x, offsetPos.y, m_width, m_height);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the Z-depth of this Widget.
    ///		@returns : 
    //-----------------------------------------------------------------------------------------------------------------------------
    unsigned int Widget::GetZOrder() const
    {
        if (!m_pParent)
            return m_zOffset;

        return m_zOffset + m_pParent->GetZOrder();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the zOffset of this widget. If the zOrder is equal to zero and we have a parent, it will be adjusted to
    ///         one.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetZOffset(const unsigned int zOffset)
    {
        // If our offset is zero and we have a parent, then we are going to make the offset 1.
        if (zOffset == 0 && m_pParent)
        {
            m_zOffset = 1;
            return;
        }
        
        m_zOffset = zOffset;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Determines whether a point of contact intersects our rect.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Widget::PointIntersectsRect(const Vec2 screenPos) const
    {
        auto rect = GetScreenRect();
        rect.x -= rect.width / 2.f;
        rect.y -= rect.height / 2.f;

        return rect.Intersects(screenPos);
    }
}
