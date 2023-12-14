// Widget.cpp

#include "Widget.h"

#include "LuaSource.h"
#include "MCP/Core/Event/ApplicationEvent.h"
#include "MCP/Debug/Assert.h"
#include "MCP/Debug/Log.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Lua/Lua.h"
#include "MCP/Scene/Scene.h"
#include "MCP/Scene/SceneManager.h"

namespace mcp
{
    Widget::Widget(const WidgetConstructionData& data)
        : SceneEntity(data.entityConstructionData)
        , m_enableBehaviorScript(data.enableBehaviorScript)
        , m_pMaskingWidget(nullptr)
        , m_localPos(data.rect.GetPosition())
        , m_pivot(data.pivot)
        , m_anchor(data.anchor)
        , m_scale(data.scale)
        , m_width(data.rect.width)
        , m_height(data.rect.height)
        , m_zOffset(0)
        , m_isInteractable(data.isInteractable)
        , m_isVisible(false)
        , m_sizedToContent(data.sizedToContent)
        , m_isMask(data.isMask)
#if MCP_EDITOR
        , m_held(false)
#endif
    {
        SetZOffset(data.zOffset);

        // HACK: Get the Current UI Layer.
        SetLayer(SceneManager::Get()->GetActiveScene()->GetUILayer());

        // HACK. I need to have an early step dedicated to initializing the 'this' pointer in Script behavior.
        m_enableBehaviorScript.Run("Init", this);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Recursively perform a task that takes in a Widget* on every child of this Widget.
    ///		@param task : Function to perform.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::ForAllChildren(const std::function<void(Widget* pWidget)>& task) const
    {
        for (auto* pChild : m_children)
        {
            auto* pWidget = SafeCastEntity<Widget>(pChild);
            pWidget->ForAllChildren(task);
            task(pWidget);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Focuses this Widget. A Focused widget and all of it's children will be recipients of input events.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::Focus()
    {
        GetUILayer()->FocusWidget(this);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Have this Widget and all of its children respond to being focused or losing focus.
    ///		@param isFocused : Whether this Widget is now focused or not.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnFocusChanged(const bool isFocused)
    {
        if (isFocused)
            OnFocus();
        else
            OnFocusLost();

        for (auto* pChild : m_children)
        {
            auto* pWidgetChild = SafeCastEntity<Widget>(pChild);
            pWidgetChild->OnFocusChanged(isFocused);
        }
    }
    
    Widget* Widget::GetChildByTag(const StringId tag) const
    {
        auto* pChild = SceneEntity::GetChildByTag(tag);

        if (!pChild)
            return nullptr;

        return SafeCastEntity<Widget>(pChild);
    }

    Widget* Widget::GetChildByTag(const StringId tag)
    {
        auto* pChild = SceneEntity::GetChildByTag(tag);

        if (!pChild)
            return nullptr;

        return SafeCastEntity<Widget>(pChild);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Sends the event down the stack to see if anyone handles it.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnEvent(ApplicationEvent& event)
    {
        // If we are not active, then don't even consult the children and return.
        // - If a parent is not active, the children are not active.
        if (!IsActive())
            return;

        // If we have children, we need to check to see if they should handle the event
        // 'Move down the tree'
        for (auto* pChild : m_children)
        {
            auto* pWidgetChild = SafeCastEntity<Widget>(pChild);
            pWidgetChild->OnEvent(event);

            // If the child handled the event, we are done.
            if (event.IsHandled())
                return;
        }

#if MCP_EDITOR
        // In the editor, we only respond to MouseEvents...
        if (event.GetEventId() == MouseButtonEvent::GetStaticId())
        {
            auto& mouseButtonEvent = static_cast<MouseButtonEvent&>(event);
            Vec2 relativeClickPos;

            if (mouseButtonEvent.State() == ButtonState::kPressed && GetPointRelativeToRect(mouseButtonEvent.GetWindowPosition(), relativeClickPos))
            {
                m_held = true;

                GetUILayer()->SetSelected(this);

                relativeClickPos.x -= m_pivot.x * GetRectWidth();
                relativeClickPos.y -= m_pivot.y * GetRectHeight();

                m_grabbedOffset = relativeClickPos;

                //MCP_LOG("Widget", "Mouse Pos On Click: ", mouseButtonEvent.GetWindowPosition().ToString());
                //MCP_LOG("Widget", "Grabbed Pos: ", m_grabbedOffset.ToString());
                event.SetHandled();
            }

            else if (mouseButtonEvent.State() == ButtonState::kReleased)
            {
                // "Let go" of the Widget.
                m_held = false;
            }

            else if (mouseButtonEvent.State() == ButtonState::kHeld && m_held)
            {
                SetPosition(mouseButtonEvent.GetWindowPosition() - m_grabbedOffset);
                event.SetHandled();
            }
        }

#else
        // If the event was not handled by any of our children and we are interactable,
        // then handle the event.
        if (!event.IsHandled() && m_isInteractable)
        {
            HandleEvent(event);
        }
#endif
        
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the positional offset from the anchor position. If this widget has no parent, then this will effectively be
    ///         its position on the screen.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetLocalPosition(const float x, const float y)
    {
        m_localPos.x = x;
        m_localPos.y = y;

        // Let the Widget Respond to the movement:
        OnMove();

        // Let our children respond as well:
        for (auto* pChild : m_children)
        {
            auto* pWidget = SafeCastEntity<Widget>(pChild);
            pWidget->OnMove();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the position of the origin at the screen space coordinates (x,y).
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetPosition(const float x, const float y)
    {
        SetPosition(Vec2(x, y));
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Set the position of the origin at the screen position.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetPosition(const Vec2 screenPos)
    {
        if (m_pParent)
        {
            const auto pParent = GetParent();
            m_localPos = screenPos - pParent->GetOrigin();

            // Adjust to our anchored position.
            m_localPos.x -= (m_anchor.x - 0.5f) * pParent->GetRectWidth();
            m_localPos.y -= (m_anchor.y - 0.5f) * pParent->GetRectHeight();
        }

        // Let the Widget Respond to the movement:
        OnMove();

        // Let our children respond as well:
        for (auto* pChild : m_children)
        {
            auto* pWidget = SafeCastEntity<Widget>(pChild);
            pWidget->OnMove();
        }
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
    ///		@brief : Returns the final width of the Widget's rect - scaled appropriately according to their parents.
    //-----------------------------------------------------------------------------------------------------------------------------
    float Widget::GetRectWidth() const
    {
        return GetScale().x * m_width;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the final height of the Widget's rect - scaled appropriately according to their parents.
    //-----------------------------------------------------------------------------------------------------------------------------
    float Widget::GetRectHeight() const
    {
        return GetScale().y * m_height;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the greatest width among this Widget's children.
    //-----------------------------------------------------------------------------------------------------------------------------
    float Widget::GetMaxChildWidth() const
    {
        float maxWidth = 0.f;
        for (const auto* pChild : m_children)
        {
            const auto* pWidget = SafeCastEntity<Widget>(pChild);
            const float widgetWidth = pWidget->GetRectWidth();
            if (widgetWidth > maxWidth)
                maxWidth = widgetWidth;
        }

        return maxWidth;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the greatest height among this Widget's children.
    //-----------------------------------------------------------------------------------------------------------------------------
    float Widget::GetMaxChildHeight() const
    {
        float maxHeight = 0.f;
        for (const auto* pChild : m_children)
        {
            const auto* pWidget = SafeCastEntity<Widget>(pChild);
            const float widgetHeight = pWidget->GetRectHeight();
            if (widgetHeight > maxHeight)
                maxHeight = widgetHeight;
        }

        return maxHeight;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the total width of all of this Widget's children. NOTE: This isn't the total width including spaces between
    ///             child widgets, just each Widget's width added together.
    //-----------------------------------------------------------------------------------------------------------------------------
    float Widget::GetTotalRectWidthOfChildren() const
    {
        float totalWidth = 0.f;
        for (const auto* pChild : m_children)
        {
            const auto* pWidget = SafeCastEntity<Widget>(pChild);

            totalWidth += pWidget->GetRectWidth();
        }

        return totalWidth;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the total height of all of this Widget's children. NOTE: This isn't the total height including spaces between
    ///             child widgets, just each Widget's height added together.
    //-----------------------------------------------------------------------------------------------------------------------------
    float Widget::GetTotalRectHeightOfChildren() const
    {
        float totalWidth = 0.f;
        for (const auto* pChild : m_children)
        {
            const auto* pWidget = SafeCastEntity<Widget>(pChild);

            totalWidth += pWidget->GetRectWidth();
        }

        return totalWidth;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the final scale of this widget; meaning the scale of its parents are taken into account.
    //-----------------------------------------------------------------------------------------------------------------------------
    Vec2 Widget::GetScale() const
    {
        if (m_pParent)
        {
            return m_scale * GetParent()->GetScale();
        }

        return m_scale;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the position of the rect centered about the origin of its parent, or  
    ///		@returns : 
    //-----------------------------------------------------------------------------------------------------------------------------
    Vec2 Widget::GetRectCenter() const
    {
        Vec2 center = GetOrigin();
        center.x += (0.5f - m_pivot.x) * GetRectWidth();
        center.y += (0.5f - m_pivot.y) * GetRectHeight();

        return center;
    }

    Vec2 Widget::GetOrigin() const
    {
        if (!m_pParent)
        {
            return m_localPos;
        }

        // The origin is going to start from the center of the parent.
        Vec2 parentCenter = GetParent()->GetRectCenter();

        // Adjust to our anchor position.
        parentCenter.x += (m_anchor.x - 0.5f) * GetParent()->GetRectWidth();
        parentCenter.y += (m_anchor.y - 0.5f) * GetParent()->GetRectHeight();

        // Offset by the local position.
        parentCenter += m_localPos;

        return parentCenter;
    }

    Vec2 Widget::GetScreenSpacePos() const
    {
        if (!m_pParent)
        {
            return m_localPos;
        }

        return m_localPos + GetParent()->GetScreenSpacePos();
    }

    RectF Widget::GetRectTopLeft() const
    {
        auto rect = GetRect();

        // Adjust the position to be at the top left corner, instead of at the origin.
        rect.x -= m_pivot.x * rect.width;
        rect.y -= m_pivot.y * rect.height;

        return rect;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns a RectF that describes where the Widget is on the screen. The x and y position of the RectF represent
    ///             the Origin of the Rect. This is the pivot point in World Space
    //-----------------------------------------------------------------------------------------------------------------------------
    RectF Widget::GetRect() const
    {
        const auto origin = GetOrigin();
        return RectF(origin.x, origin.y, this->GetRectWidth(), this->GetRectHeight());
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the RectF whose position is at the top left of the rect. If this Widget is being masked to a parent,
    ///         then this returns the intersection of the Parent's Rect and this Widget's Rect.
    //-----------------------------------------------------------------------------------------------------------------------------
    RectF Widget::GetVisibleRect() const
    {
        auto rect = GetRect();

        // Adjust the position to be at the top left corner, instead of at the origin.
        rect.x -= m_pivot.x * rect.width;
        rect.y -= m_pivot.y * rect.height;

        // If this rect is being masked, we need to adjust based on that mask.
        if (m_pMaskingWidget)
        {
            const auto maskRect = m_pMaskingWidget->GetVisibleRect();

            // Get the rect as an intersection of the mask and the rect.
            rect = rect.GetIntersectionAsRect(maskRect);
        }

        return rect;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the Z-depth of this Widget.
    //-----------------------------------------------------------------------------------------------------------------------------
    int Widget::GetZOffset() const
    {
        if (!m_pParent)
            return m_zOffset;

        return m_zOffset + GetParent()->GetZOffset();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the ZOffset value of the lowest child.
    //-----------------------------------------------------------------------------------------------------------------------------
    int Widget::GetMaxZ() const
    {
        int maxZ = GetZOffset();

        if (m_children.empty())
            return maxZ;

        for (const auto* pChild : m_children)
        {
            const auto* pWidgetChild = SafeCastEntity<Widget>(pChild);
            const int childMax = pWidgetChild->GetMaxZ();
            if (childMax > maxZ)
                maxZ = childMax;
        }

        return maxZ;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the zOffset of this widget. If the zOrder is equal to zero and we have a parent, it will be adjusted to
    ///         one, making the zOffset of the child always above its parent.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetZOffset(const int zOffset)
    {
        // If our offset is less than or equal to zero and we have a parent, then we are going to make the offset 1.
        if (m_pParent &&  zOffset <= 0)
        {
            m_zOffset = 1;
            return;
        }
        
        m_zOffset = zOffset;

        // Let our children know of the change so they can respond.
        OnZSet();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Starting with this Widget, respond to the ZChange and then recursively have each child respond.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnZSet()
    {
        // Respond to the ZChange.
        OnZChanged();

        // Have each child respond.
        for (auto* pChild : m_children)
        {
            auto* pWidgetChild = SafeCastEntity<Widget>(pChild);
            pWidgetChild->OnZSet();    
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Determines whether a point of contact intersects our rect.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Widget::PointIntersectsRect(const Vec2 screenPos) const
    {
        const auto rect = GetVisibleRect();
        return rect.Intersects(screenPos);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Calculates the relative position of the of the screen point to our rect, and returns whether the point is in
    ///         our rect.
    ///		@param screenPos : Position on the screen of the point.
    ///		@param relativePos : Out param, the position of the point relative to our rect.
    ///		@returns : True of the point intersects our rect or not.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Widget::GetPointRelativeToRect(const Vec2 screenPos, Vec2& relativePos) const
    {
        const auto rect = GetVisibleRect();

        relativePos = screenPos;
        relativePos.x -= rect.x;
        relativePos.y -= rect.y;

        return rect.Intersects(screenPos);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns if this Widget responds to an input events. If the Widget is Inactive, this will always return false.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Widget::IsInteractable() const
    {
        if (!IsActive())
            return false;

        return m_isInteractable;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns whether this Widget is the focused widget or not. NOTE: Even if the widget is the child of the Focused widget,
    ///         this returns false; it only pertains to the specifically focused Widget.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Widget::IsFocused() const
    {
        return this == GetUILayer()->GetFocused();
    }

    UILayer* Widget::GetUILayer() const
    {
        return SceneLayer::SafeCastLayer<UILayer>(GetLayer());
    }

    void Widget::SetInteractable(const bool isInteractable)
    {
        if (m_isInteractable == isInteractable)
            return;

        // If I was previously interactable we need to disable it.
        if (m_isInteractable)
        {
            OnDisable();
            m_enableBehaviorScript.Run("OnEnable");
        }

        // If I was previously NOT interactable, we need to enable it.
        else
        {
            OnEnable();
            m_enableBehaviorScript.Run("OnDisable");
        }

        m_isInteractable = false;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns if this Widget is being rendered or not. If the Widget is Inactive, this will always return false.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Widget::IsVisible() const
    {
        if (!IsActive())
            return false;

        return m_isVisible;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the relative position of the origin for this Widget. The x and y values should range from [0,1]. 
    ///         An origin of (0,0) would put the origin at the top left of the Rect. An origin of (1,1) would put it at the bottom right
    ///         of the rect.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetPivot(const Vec2 pivot)
    {
        SetPivot(pivot.x, pivot.y);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the relative position of the origin for this Widget. The x and y values should range from [0,1]. 
    ///         An origin of (0,0) would put the origin at the top left of the Rect. An origin of (1,1) would put it at the bottom right
    ///         of the rect.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetPivot(float x, float y)
    {
        x = std::clamp(x, 0.f, 1.f);
        y = std::clamp(y, 0.f, 1.f);
        m_pivot.x = x;
        m_pivot.y = y;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the base width of this Widget.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetWidth(const float width)
    {
        m_width = width;

        if (m_pParent)
            GetParent()->OnChildSizeChanged();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the base height of this Widget.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetHeight(const float height)
    {
        m_height = height;

        if (m_pParent)
            GetParent()->OnChildSizeChanged();
    }

    void Widget::SetScale(const float uniformScale)
    {
        SetScale(uniformScale, uniformScale);
    }

    void Widget::SetScale(const float xScale, const float yScale)
    {
        m_scale.x = xScale;
        m_scale.y = yScale;

        if (m_pParent)
            GetParent()->OnChildSizeChanged();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set whether the rect dimensions of this Widget should be used as a mask for its children.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetIsMask(const bool isMask)
    {
        if (isMask == m_isMask)
            return;

        m_isMask = isMask;
        Widget* pMask = m_isMask ? this : nullptr;

        for (auto* pChild : m_children)
        {
            auto* pWidget = SafeCastEntity<Widget>(pChild);
            pWidget->UpdateMaskingWidget(pMask);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Returns whether this Widget is being used as a mask for its children.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Widget::IsMask() const
    {
        return m_isMask;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns whether the 'visible rect' (the rect with any applied mask) is different from it's full rect.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Widget::IsClipped() const
    {
        const auto rect = GetRect();
        auto visibleRect = rect;

        // If this rect is being masked, we need to adjust based on that mask.
        if (m_pMaskingWidget)
        {
            // Adjust the position to be at the top left corner, instead of at the origin.
            visibleRect.x -= m_pivot.x * visibleRect.width;
            visibleRect.y -= m_pivot.y * visibleRect.height;

            const auto maskRect = m_pMaskingWidget->GetVisibleRect();

            // Get the rect as an intersection of the mask and the rect.
            visibleRect = visibleRect.GetIntersectionAsRect(maskRect);

            // Return whether the two rect dimensions are equal.
            return CheckEqualFloats(visibleRect.width, rect.width)
                && CheckEqualFloats(visibleRect.height, rect.height);
        }

        // If we don't have a clipping mask, then we aren't clipped.
        return false;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the mask that the parent is using. This can be nullptr if no masking widget is present.
    //-----------------------------------------------------------------------------------------------------------------------------
    Widget* Widget::GetParentMask() const
    {
        if (m_pParent)
        {
            auto* pWidget = SafeCastEntity<Widget>(m_pParent);
            if (pWidget->IsMask())
                return pWidget;

            return pWidget->m_pMaskingWidget;
        }

        return nullptr;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Update the m_pMaskingWidget variable for each child, recursively.
    ///		@param pMaskingWidget : The Widget who we are going to use as the mask, or, if nullptr, we are removing it as a mask.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::UpdateMaskingWidget(Widget* pMaskingWidget)
    {
        m_pMaskingWidget = pMaskingWidget;

        // If this Widget is a mask, we don't need to update our children, they are already using us as a mask.
        if (m_isMask)
            return;

        for (auto* pChild : m_children)
        {
            auto* pWidget = SafeCastEntity<Widget>(pChild);
            pWidget->UpdateMaskingWidget(pMaskingWidget);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : By default, when a Widget is set Active, if we have an EnableBehavior script, it will call the OnEnable function
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnActive()
    {
        m_enableBehaviorScript.Run("OnEnable");
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : By default, when a Widget is set InActive, if we have an EnableBehavior script, it will call the OnDisable function
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnInactive()
    {
        m_enableBehaviorScript.Run("OnDisable");
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Function to respond to the Widget's position changing. Runtime: Base method is empty. Editor: Base method adds
    ///         the widget to the save buffer.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnMove()
    {
#if MCP_EDITOR
        if (!m_dirty && GetLayer()->GetState() == SceneLayer::LayerState::kRunning)
        {
            m_dirty = true;
            GetLayer()->AddToSaveBuffer(GetId());
        }
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : By default, when a Widget's parent is set, we need to force update their Z-Offset.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnParentSet()
    {
        // Update our zOffset
        SetZOffset(m_zOffset);

        // Update our masking behavior
        UpdateMaskingWidget(GetParentMask());
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : SceneEntity override of responding to adding child. We call into the OnChildAdded that takes in a Widget*.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnChildAdded(SceneEntity* pChild)
    {
        MCP_CHECK(pChild->GetLayerId() == Widget::GetStaticLayerId());

        auto* pChildWidget = static_cast<Widget*>(pChild);
        OnChildAdded(pChildWidget);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : SceneEntity override of responding to removing child. We call into the OnChildAdded that takes in a Widget*.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnChildRemoved(SceneEntity* pChild)
    {
        MCP_CHECK(pChild->GetLayerId() == Widget::GetStaticLayerId());

        auto* pChildWidget = static_cast<Widget*>(pChild);
        OnChildRemoved(pChildWidget);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : By default, we notify of the Z-Change of the Child.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnChildAdded(Widget* pChild)
    {
        pChild->OnZChanged();
    }

#if MCP_EDITOR
    void Widget::Save()
    {
        MCP_CHECK(m_entityRoot.IsValid());

        // Save our positional data:
        auto rectElement = m_entityRoot.GetChildElement("Rect");
        MCP_CHECK(rectElement.IsValid());

        rectElement.SetAttribute("x", m_localPos.x);
        rectElement.SetAttribute("y", m_localPos.y);
    }
#endif

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Gets the basic construction data from a widget element-The dimensions, anchor, tag, etc.
    //-----------------------------------------------------------------------------------------------------------------------------
    WidgetConstructionData Widget::GetWidgetConstructionData(const XMLElement widgetElement)
    {
        // Get the Widget information:
        WidgetConstructionData data;
        data.entityConstructionData = GetEntityConstructionData(widgetElement);

        data.isInteractable = widgetElement.GetAttributeValue<bool>("isInteractable", false);
        data.zOffset = widgetElement.GetAttributeValue<int>("zOffset", 1);
        data.isMask = widgetElement.GetAttributeValue<bool>("isMask", false);

        //data.startEnabled = widgetElement.GetAttributeValue<bool>("startEnabled", true);

        // Tag. Optional element.
        const char* pTag = widgetElement.GetAttributeValue<const char*>("tag", nullptr);
        if (pTag)
                data.tag = StringId(pTag);

        // Rect. This must be present for Widgets to work properly.
        auto childElement = widgetElement.GetChildElement("Rect");
        MCP_CHECK_MSG(childElement.IsValid(), "Failed to load WidgetConstructionData! No 'Rect' element was found!");

        data.rect.x = childElement.GetAttributeValue<float>("x");
        data.rect.y = childElement.GetAttributeValue<float>("y");
        data.rect.width = childElement.GetAttributeValue<float>("width");
        data.rect.height = childElement.GetAttributeValue<float>("height");

        // TODO: This should be moved to its own Element.
        data.anchor.x = childElement.GetAttributeValue<float>("anchorX", 0.5f);
        data.anchor.y = childElement.GetAttributeValue<float>("anchorY", 0.5f);

        // Pivot
        childElement = widgetElement.GetChildElement("Pivot");
        if (childElement.IsValid())
        {
            data.pivot.x = childElement.GetAttributeValue<float>("x", 0.5f);
            data.pivot.y = childElement.GetAttributeValue<float>("y", 0.5f);
        }

        // Scale.
        childElement = widgetElement.GetChildElement("Scale");
        if (childElement.IsValid())
        {
            data.scale.x = childElement.GetAttributeValue<float>("x", 1.f);
            data.scale.y = childElement.GetAttributeValue<float>("y", 1.f);
            data.sizedToContent = childElement.GetAttributeValue<bool>("sizedToContent", false);
        }

        // Optional Enable Behavior Script
        childElement = widgetElement.GetChildElement("EnableBehavior");
        if (childElement.IsValid())
        {
            const char* pScriptPath = childElement.GetAttributeValue<const char*>("scriptPath");
            const char* pScriptDataPath = childElement.GetAttributeValue<const char*>("scriptData");
            data.enableBehaviorScript = lua::LoadScriptInstance(pScriptPath, pScriptDataPath);
        }
        
        return data;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Sets the given widget active or not.
    ///
    ///     \n LUA PARAMS: Widget* pWidget, const bool isActive
    ///     \n RETURNS: VOID
    //-----------------------------------------------------------------------------------------------------------------------------
    static int SetWidgetActive(lua_State* pState)
    {
        // Get the Widget and bool parameters off the stack.
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);
        const bool isActive = lua_toboolean(pState, -1);

        // Pop the two parameters
        lua_pop(pState, 2);

        //MCP_LOG("Lua", "Setting ", *pWidget->GetTag(), " to ", isActive);
        // Set the active state of the Widget.
        pWidget->SetActive(isActive);

        //MCP_LOG("Lua", "Stack Size = ", lua_gettop(pState));

        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Sets the given widget as the focused Widget in the Scene.
    ///
    ///     \n LUA PARAMS: Widget* pWidget
    ///     \n RETURNS: VOID
    //-----------------------------------------------------------------------------------------------------------------------------
    static int FocusWidget(lua_State* pState)
    {
        //MCP_LOG("Lua", "Calling FocusWidget() from lua...");

        // Get the Widget
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -1));
        MCP_CHECK(pWidget);

        // Pop the param
        lua_pop(pState, 1);

        // Focus this widget.
        pWidget->Focus();

        //MCP_LOG("Lua", "Stack Size = ", lua_gettop(pState));

        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Sets the anchor for the given widget.
    ///
    ///     \n LUA PARAMS: Widget* pWidget, const float anchorX, const float anchorY
    ///     \n RETURNS: VOID
    //-----------------------------------------------------------------------------------------------------------------------------
    static int SetWidgetAnchor(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -3));
        MCP_CHECK(pWidget);

        auto anchorX = static_cast<float>(lua_tonumber(pState, -2));
        auto anchorY = static_cast<float>(lua_tonumber(pState, -1));

        anchorX = std::clamp(anchorX, 0.f, 1.f);
        anchorY = std::clamp(anchorY, 0.f, 1.f);

        // Pop the parameters off the stack.
        lua_pop(pState, 3);

        // Set the anchor.
        pWidget->SetAnchor(anchorX, anchorY);

        return 0;
    }

    static int GetWidgetLocalPosition(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -1));
        MCP_CHECK(pWidget);

        // Pop the param.
        lua_pop(pState, 1);

        const auto position = pWidget->GetLocalPosition();

        // Push the position x and y onto the stack.
        lua_pushnumber(pState, static_cast<double>(position.x));
        lua_pushnumber(pState, static_cast<double>(position.y));

        return 2;
    }


    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Sets the position for the given widget.
    ///
    ///     \n LUA PARAMS: Widget* pWidget, const float x, const float y
    ///     \n RETURNS: VOID
    //-----------------------------------------------------------------------------------------------------------------------------
    static int SetWidgetPosition(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -3));
        MCP_CHECK(pWidget);

        const auto x = static_cast<float>(lua_tonumber(pState, -2));
        const auto y = static_cast<float>(lua_tonumber(pState, -1));

        // Pop the parameters off the stack.
        lua_pop(pState, 3);

        // Set the anchor.
        pWidget->SetLocalPosition(x, y);

        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the width and height for the given widget.
    ///
    ///     \n LUA PARAMS: Widget* pWidget
    ///     \n RETURNS: width & height
    //-----------------------------------------------------------------------------------------------------------------------------
    static int GetWidgetDimensions(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -1));
        MCP_CHECK(pWidget);

        // Pop the param.
        lua_pop(pState, 1);

        const auto rect = pWidget->GetRect();

        // Push the width and height onto the stack.
        lua_pushnumber(pState, static_cast<double>(rect.width));
        lua_pushnumber(pState, static_cast<double>(rect.height));

        return 2;
    }

    static int SetWidgetRectWidth(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);

        // Get the Width
        const auto width = static_cast<float>(lua_tonumber(pState, -1)); 

        // Pop the Params
        lua_pop(pState, 2);

        // Set the Width
        pWidget->SetWidth(width);

        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Adds the passed in widget to the UI stack in the UILayer.
    ///
    ///     \n LUA PARAMS: Widget* pWidget
    ///     \n NO RETURN.
    //-----------------------------------------------------------------------------------------------------------------------------
    static int AddWidgetToStack(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -1));
        MCP_CHECK(pWidget);

        // Pop the param.
        lua_pop(pState, 1);

        pWidget->GetUILayer()->AddToStack(pWidget);

        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Pop the top Widget off the UILayer's stack.
    ///
    ///     \n LUA PARAMS: Widget* pWidget
    ///     \n NO RETURN.
    //-----------------------------------------------------------------------------------------------------------------------------
    static int PopWidgetStack(lua_State* pState)
    {
        // Get the Widget
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -1));
        MCP_CHECK(pWidget);

        // Pop the param.
        lua_pop(pState, 1);

        pWidget->GetUILayer()->PopStack();
        return 0;
    }

    void Widget::RegisterLuaFunctions(lua_State* pState)
    {
        static constexpr luaL_Reg kWidgetFuncs[]
        {
            {"SetActive", &SetWidgetActive}
            ,{"Focus", &FocusWidget }
            , { "SetWidth", &SetWidgetRectWidth }
            , {"SetAnchor", &SetWidgetAnchor }
            , {"SetPosition", &SetWidgetPosition }
            , {"GetLocalPosition", &GetWidgetLocalPosition}
            , {"GetDimensions", &GetWidgetDimensions }
            , {"AddToLayerStack", &AddWidgetToStack }
            , {"PopLayerStack", &PopWidgetStack }
            ,{nullptr, nullptr}
        };

        // Get the global Widget library class,
        lua_getglobal(pState, "Widget");
        MCP_CHECK(lua_type(pState, -1) == LUA_TTABLE);

        // Set its functions
        luaL_setfuncs(pState, kWidgetFuncs, 0);

        // Pop the table off the stack.
        lua_pop(pState, 1);

        //MCP_LOG("Lua", "Stack Size = ", lua_gettop(pState));
    }
}
