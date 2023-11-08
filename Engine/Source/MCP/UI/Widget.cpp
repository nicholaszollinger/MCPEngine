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
        : m_pUILayer(nullptr)
        , m_tag(data.tag)
        , m_pParent(nullptr)
        , m_enableBehaviorScript(data.enableBehaviorScript)
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
        , m_isActive(data.startEnabled)
    {
        SetZOffset(data.zOffset);

        // Get the Current UI Layer.
        m_pUILayer = SceneManager::Get()->GetActiveScene()->GetUILayer();

        // HACK. I need to have an early step dedicated to initializing the 'this' pointer in Script behavior.
        if (m_enableBehaviorScript.IsValid())
            lua::CallMemberFunction(m_enableBehaviorScript, "Init", this);
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

            m_pParent = pParent;
            SetZOffset(m_zOffset); // Update our zOffset
            OnParentSet();
        }
    }

    void Widget::AddChild(Widget* pChild)
    {
        m_children.push_back(pChild);
        pChild->m_pParent = this;

        pChild->OnParentSet();
        pChild->OnZChanged();
        pChild->OnParentActiveChanged(IsActive());
        OnChildAdded(pChild);
    }

    void Widget::RemoveChild(Widget* pChild)
    {
        for (size_t i = 0; i < m_children.size(); ++i)
        {
            if (pChild == m_children[i])
            {
                std::swap(m_children[i], m_children.back());
                m_children.pop_back();
                OnChildRemoved(pChild);
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
        if (m_pParent)
        {
            // Set each child's parent to be this Widget's parent, and add the child to that parent's children.
            for (auto* pChild : m_children)
            {
                pChild->SetParent(m_pParent);
                m_pParent->AddChild(pChild);
            }
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
    ///		@brief : Recursively perform a task that takes in a Widget* on every child of this Widget.
    ///		@param task : Function to perform.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::ForAllChildren(const std::function<void(Widget* pWidget)>& task) const
    {
        for (auto* pChild : m_children)
        {
            pChild->ForAllChildren(task);
            task(pChild);
        }
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
            pChild->OnFocusChanged(isFocused);
        }
    }
    
    Widget* Widget::FindChildByTag(const StringId tag) const
    {
        for (auto* pChild : m_children)
        {
            if (pChild->GetTag() == tag)
            {
                return pChild;
            }

            if (Widget* pFound = pChild->FindChildByTag(tag))
                return pFound;
        }

        return nullptr;
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
        if (!m_isActive)
            return;

        // If we have children, we need to check to see if they should handle the event
        // 'Move down the tree'
        for (auto* pChild : m_children)
        {
            pChild->OnEvent(event);

            // If the child handled the event, we are done.
            if (event.IsHandled())
                return;
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
    void Widget::SetLocalPosition(const float x, const float y)
    {
        m_localPos.x = x;
        m_localPos.y = y;
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
    ///		@brief : Returns the final scale of this widget; meaning the scale of its parents are taken into account.
    //-----------------------------------------------------------------------------------------------------------------------------
    Vec2 Widget::GetScale() const
    {
        if (m_pParent)
        {
            return m_scale * m_pParent->GetScale();
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
        Vec2 parentCenter = m_pParent->GetRectCenter();

        // Adjust to our anchor position.
        parentCenter.x += (m_anchor.x - 0.5f) * m_pParent->GetRectWidth();
        parentCenter.y += (m_anchor.y - 0.5f) * m_pParent->GetRectHeight();

        // Offset by the local position.
        parentCenter += m_localPos;

        return parentCenter;
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
    ///		@brief : Returns the RectF whose position is at the top left of the rect. This is useful for rendering and collision.
    //-----------------------------------------------------------------------------------------------------------------------------
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
    ///		@brief : Get the Z-depth of this Widget.
    //-----------------------------------------------------------------------------------------------------------------------------
    int Widget::GetZOffset() const
    {
        if (!m_pParent)
            return m_zOffset;

        return m_zOffset + m_pParent->GetZOffset();
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
            const int childMax = pChild->GetMaxZ();
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
            pChild->OnZSet();    
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Determines whether a point of contact intersects our rect.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Widget::PointIntersectsRect(const Vec2 screenPos) const
    {
        const auto rect = GetRectTopLeft();
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
        const auto rect = GetRectTopLeft();

        relativePos = screenPos;
        relativePos.x -= rect.x;
        relativePos.y -= rect.y;

        return rect.Intersects(screenPos);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      This tells the children of the active state change of the Parent Widget, so they can respond without
    //      changing their active state necessarily.
    //		
    ///		@brief : Sets the Widget active or not.
    ///		@param isActive : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::SetActive(const bool isActive)
    {
        // If our status isn't changing, return.
        if (isActive == m_isActive)
            return;

        m_isActive = isActive;

        // If we have a parent, we need to check with their active state before updating our active state.
        if (m_pParent)
        {
            // If our parent was active and our state has changed, then we need to update.
            if (m_pParent->IsActive())
            {
                if (m_isActive)
                {
                    OnActive();

                    // If we were interactable, we need to have the script respond to OnEnable.
                    if (m_enableBehaviorScript.IsValid())
                        lua::CallMemberFunction(m_enableBehaviorScript, "OnEnable");
                }

                // If we have been turned off:
                else
                {
                    OnInactive();

                    // If we were interactable, we need to have the script respond to OnDisable.
                    if (m_enableBehaviorScript.IsValid())
                        lua::CallMemberFunction(m_enableBehaviorScript, "OnDisable");
                }
            }
        }

        // Otherwise we update according to our active state.
        else
        {
            // If we have been set active.
            if (m_isActive)
            {
                OnActive();

                // If we were interactable, we need to have the script respond to OnEnable.
                if (m_enableBehaviorScript.IsValid())
                    lua::CallMemberFunction(m_enableBehaviorScript, "OnEnable");
            }

            // If we have been turned off:
            else 
            {
                OnInactive();

                // If we were interactable, we need to have the script respond to OnDisable.
                if (m_enableBehaviorScript.IsValid())
                    lua::CallMemberFunction(m_enableBehaviorScript, "OnDisable");
            }
        }

        // Let our children know of the change.
        for (auto* pChild : m_children)
        {
            pChild->OnParentActiveChanged(m_isActive);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Toggles the active state on the Widget.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::ToggleActive()
    {
        SetActive(!m_isActive);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Respond to our parent widget's active state changing, and let all of our children know as well recursively.
    ///		@param parentActiveState : New state of the parent.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnParentActiveChanged(const bool parentActiveState)
    {
        // If we are active and our parent has changed, we need to respond.
        // We also need to check if we have a valid UILayer. This is because if we are being created as child of a Widget
        // that isn't a part of the Layer yet, we don't have access to the UILayer too. This becomes an issue for renderable
        // Widgets, when they try to access Layer's Renderable Container. Plus, this will be called when the parent is added
        // anyway.
        if (m_isActive)
        {
            if (parentActiveState)
            {
                OnActive();
                // We need to have the script respond to OnEnable.
                if (m_enableBehaviorScript.IsValid())
                    lua::CallMemberFunction(m_enableBehaviorScript, "OnEnable");
            }

            else
            {
                OnInactive();
                // We need to have the script respond to OnDisable.
                if (m_enableBehaviorScript.IsValid())
                    lua::CallMemberFunction(m_enableBehaviorScript, "OnDisable");
            }
        }

        for (auto* pChild : m_children)
        {
            pChild->OnParentActiveChanged(parentActiveState);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns if the Widget is active or not. If a parent Widget is inactive, then this will return inactive.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Widget::IsActive() const
    {
        if (m_pParent && !m_pParent->IsActive())
            return false;

        return m_isActive;
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
        return this == m_pUILayer->GetFocused();
    }

    void Widget::SetInteractable(const bool isInteractable)
    {
        if (m_isInteractable == isInteractable)
            return;

        // If I was previously interactable we need to disable it.
        if (m_isInteractable)
        {
            OnDisable();
            if (m_enableBehaviorScript.IsValid())
                    lua::CallMemberFunction(m_enableBehaviorScript, "OnEnable");
        }

        // If I was previously NOT interactable, we need to enable it.
        else
        {
            OnEnable();
            if (m_enableBehaviorScript.IsValid())
                    lua::CallMemberFunction(m_enableBehaviorScript, "OnDisable");
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
            m_pParent->OnChildSizeChanged();
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
            m_pParent->OnChildSizeChanged();
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
            m_pParent->OnChildSizeChanged();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Gets the basic construction data from a widget element-The dimensions, anchor, tag, etc.
    //-----------------------------------------------------------------------------------------------------------------------------
    WidgetConstructionData Widget::GetWidgetConstructionData(const XMLElement widgetElement)
    {
        // Get the Widget information:
        WidgetConstructionData data;
        data.isInteractable = widgetElement.GetAttributeValue<bool>("isInteractable", false);
        data.zOffset = widgetElement.GetAttributeValue<int>("zOffset", 1);
        data.startEnabled = widgetElement.GetAttributeValue<bool>("startEnabled", true);

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

        pWidget->GetLayer()->AddToStack(pWidget);

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

        pWidget->GetLayer()->PopStack();
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
