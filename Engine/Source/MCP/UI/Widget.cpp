// Widget.cpp

#include "Widget.h"

#include "LuaSource.h"
#include "MCP/Core/Event/ApplicationEvent.h"
#include "MCP/Debug/Assert.h"
#include "MCP/Debug/Log.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Lua/Lua.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    Widget::Widget(const WidgetConstructionData& data)
        : m_pUILayer(nullptr)
        , m_tag(data.tag)
        , m_pParent(nullptr)
        , m_enableBehaviorScript(data.enableBehaviorScript)
        , m_offset(data.rect.GetPosition())
        , m_anchor(data.anchor)
        , m_width(data.rect.width)
        , m_height(data.rect.height)
        , m_zOffset(0)
        , m_isInteractable(data.isInteractable)
        , m_isActive(data.startEnabled)
    {
        SetZOffset(data.zOffset);

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
        }
    }

    void Widget::AddChild(Widget* pChild)
    {
        m_children.push_back(pChild);
        pChild->m_pParent = this;
        pChild->OnParentActiveChanged(IsActive());
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
        if (isActive == m_isActive)
            return;

        m_isActive = isActive;
        if (m_isActive)
        {
            OnEnable();
            if (m_enableBehaviorScript.IsValid())
                lua::CallMemberFunction(m_enableBehaviorScript, "OnEnable");
        }

        else
        {
            OnDisable();
            if (m_enableBehaviorScript.IsValid())
                lua::CallMemberFunction(m_enableBehaviorScript, "OnDisable");
        }

        for (auto* pChild : m_children)
        {
            pChild->OnParentActiveChanged(m_isActive);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Respond to our parent widget's active state changing, and let all of our children know as well recursively.
    ///		@param parentActiveState : New state of the parent.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Widget::OnParentActiveChanged(const bool parentActiveState)
    {
        // If we are supposed to be active,
        if (m_isActive)
        {
            if (parentActiveState)
            {
                OnEnable();
                if (m_enableBehaviorScript.IsValid())
                    lua::CallMemberFunction(m_enableBehaviorScript, "OnEnable");
            }

            else
            {
                OnDisable();
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
        data.anchor.x = childElement.GetAttributeValue<float>("anchorX", 0.5f);
        data.anchor.y = childElement.GetAttributeValue<float>("anchorY", 0.5f);

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

        // Get the Widget and bool parameters off the stack.
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -1));
        MCP_CHECK(pWidget);

        // Pop the param
        lua_pop(pState, 1);

        // Focus this widget.
        pWidget->Focus();

        return 0;
    }

    void Widget::RegisterLuaFunctions(lua_State* pState)
    {
        lua_pushcfunction(pState, &SetWidgetActive);
        lua_setglobal(pState, "SetWidgetActive");

        lua_pushcfunction(pState, &FocusWidget);
        lua_setglobal(pState, "FocusWidget");
    }
}
