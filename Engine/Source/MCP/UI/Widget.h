#pragma once
// WidgetComponent.h

#include <vector>

#include "WidgetFactory.h"
#include "MCP/Lua/LuaResource.h"
#include "Utility/String/StringId.h"
#include "Utility/Types/Rect.h"

struct lua_State;

namespace mcp
{
    class ApplicationEvent;
    class MouseButtonEvent;
    class MouseMoveEvent;
    class Scene;
    class UILayer;
    class Widget;

#define MCP_DEFINE_WIDGET(widgetTypeName)                                                                                    \
    private:                                                                                                                 \
        static inline const mcp::WidgetTypeId kId = mcp::WidgetFactory::RegisterWidgetType<widgetTypeName>(#widgetTypeName); \
                                                                                                                             \
    public:                                                                                                                  \
        static mcp::WidgetTypeId GetStaticTypeId() { return kId; }                                                           \
        virtual mcp::WidgetTypeId GetTypeId() const override { return kId; }                                                 \
    private:             

    struct WidgetConstructionData
    {
        RectF rect {};                           // Position of the rect is the offset from the parent, or the actual screen position if the parent is nullptr.
        Vec2 anchor = {0.5f, 0.5f};         // How the Widget is anchored to it's parent. Default is to anchor to the center.
        StringId tag = {};                       // Optional tag to give the Widget.
        LuaResourcePtr enableBehaviorScript;     // Optional script that defines extra behavior for enabling and disabling this widget.
        unsigned int zOffset = 1;                // z-Axis difference from the parent.
        bool isInteractable;                     // Whether this Widget is interactable or not.
        bool startEnabled = true;                // Whether this Widget is active when loaded.
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Base class for any UI Components.
    //-----------------------------------------------------------------------------------------------------------------------------
    class Widget
    {
        friend class LuaSystem;
    protected:
        static constexpr StringId kInvalidTag = StringId();

        std::vector<Widget*> m_children;    // Child widgets. Their positions are offsets 
        UILayer* m_pUILayer;                // Reference to the UILayer that we are in.
        const StringId m_tag;               // Optional tag to be able to find this specific widget.
        Widget* m_pParent;                  // Parent of this widget. If the parent is nullptr, then it is the Root.
        LuaResourcePtr m_enableBehaviorScript;  // Optional Script that defines behavior when this script is enabled or disabled.
        Vec2 m_offset;                      // Offset from the anchor position 
        Vec2 m_anchor;                      // Determines the position of Widget based on the Parent's position. An anchor can have values from [0, 1]
        float m_width;                      // Width of the Widget.
        float m_height;                     // Height of the Widget.
        unsigned int m_zOffset;             // The z-Depth of this widget
        bool m_isInteractable;

    private:
        bool m_isActive;     // Whether this widget is active or not. Use IsActive() to properly check to if you are active or not.

    public: 
        Widget(const WidgetConstructionData& data);
        virtual ~Widget() = default;

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called right after construction, no Potential parent is set and child widgets will not be created or set yet!
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual bool Init() { return true; }

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called once the layer is loaded. All scene entities loaded from data have been created.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual bool PostLoadInit() { return true; }

        void SetParent(Widget* pParent);
        void AddChild(Widget* pChild); // Should this handle the creation of the widget and return the child?
        void RemoveChild(Widget* pChild);
        void DestroyWidget();
        void DestroyWidgetAndChildren();
        void Focus();

        // Querying Children
        template<typename WidgetType> WidgetType* FindChildByTag(const StringId tag);
        template<typename WidgetType> WidgetType* FindFirstChildOfType();
        [[nodiscard]] const std::vector<Widget*>& GetChildren() const { return m_children; }
        void ForAllChildren(const std::function<void(Widget* pWidget)>& task) const;

        // Event Handling
        void OnEvent(ApplicationEvent& event);

        // Setters
        virtual void SetPositionOffset(const float x, const float y);
        void SetAnchor(const float x, const float y);
        void SetZOffset(const unsigned int zOffset);
        void SetActive(const bool isActive);
        void SetUILayer(UILayer* pUILayer) { m_pUILayer = pUILayer; }

        // Getters
        [[nodiscard]] StringId GetTag() const { return m_tag; }
        [[nodiscard]] RectF GetScreenRect() const;
        [[nodiscard]] unsigned int GetZOrder() const;
        [[nodiscard]] bool HasChildren() const { return !m_children.empty(); }
        [[nodiscard]] bool IsInteractable() const { return m_isInteractable; }
        [[nodiscard]] bool IsActive() const;
        [[nodiscard]] virtual WidgetTypeId GetTypeId() const = 0;
        [[nodiscard]] UILayer* GetLayer() const { return m_pUILayer; }

    protected:
        static WidgetConstructionData GetWidgetConstructionData(const XMLElement widgetElement);
        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Virtual function to override how this Widget handles Application Events. The default behavior is to do nothing.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void HandleEvent([[maybe_unused]] ApplicationEvent& event) {}

        virtual void OnEnable() {}
        virtual void OnDisable() {}
        virtual void OnChildAdded() {}
        virtual void OnChildRemoved() {}
        [[nodiscard]] bool PointIntersectsRect(const Vec2 screenPos) const;

    private:
        void OnParentActiveChanged(const bool parentActiveState);
        static void RegisterLuaFunctions(lua_State* pState);
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Recursively tries to find a child Widget that is of WidgetType and matches the tag.
    ///		@returns : Nullptr if no child matching the criteria was found.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename WidgetType>
    WidgetType* Widget::FindChildByTag(const StringId tag)
    {
        for (auto* pChild : m_children)
        {
            // If it matches this child, return the child casted to the correct type.
            if (pChild->GetTypeId() == WidgetType::GetStaticTypeId() && tag == pChild->GetTag())
            {
                return static_cast<WidgetType*>(pChild);
            }

            // Recursively check the children.
            if (WidgetType* pResult = pChild->FindChildByTag<WidgetType>(tag))
                return pResult;
        }

        // If we didn't have a child by that tag of that type, return nullptr.
        return nullptr;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Finds the first Child that matches the WidgetType.
    ///		@returns : Nullptr if this Widget has no children of that type
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename WidgetType>
    WidgetType* Widget::FindFirstChildOfType()
    {
        for (auto* pChild : m_children)
        {
            // If it matches this child, return the child casted to the correct type.
            if (pChild->GetTypeId() == WidgetType::GetStaticTypeId())
            {
                return static_cast<WidgetType*>(pChild);
            }

            // Recursively check the children.
            if (WidgetType* pResult = pChild->FindFirstChildOfType<WidgetType>())
                return pResult;
        }

        // If we didn't have a child by that tag of that type, return nullptr.
        return nullptr;
    }


}
