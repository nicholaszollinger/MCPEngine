#pragma once
// WidgetComponent.h

#include <vector>

#include "WidgetFactory.h"
//#include "MCP/Components/Component.h"
#include "Utility/Types/Rect.h"

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
        RectF rect;                         // Position of the rect is the offset from the parent, or the actual screen position if the parent is nullptr.
        Vec2 anchor = {0.5f, 0.5f};    // How the Widget is anchored to it's parent. Default is to anchor to the center.
        unsigned int zOffset = 1;           // z-Axis difference from the parent.
        bool isInteractable;                // Whether this Widget is interactable or not.
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Base class for any UI Components.
    //-----------------------------------------------------------------------------------------------------------------------------
    class Widget
    {
    protected:
        std::vector<Widget*> m_children;    // Child widgets. Their positions are offsets 
        UILayer* m_pUILayer;                // Reference to the UILayer that we are in.
        Widget* m_pParent;                  // Parent of this widget. If the parent is nullptr, then it is the Root.
        Vec2 m_offset;                      // Offset from the anchor position 
        Vec2 m_anchor;                      // Determines the position of Widget based on the Parent's position. An anchor can have values from [0, 1]
        float m_width;                      // Width of the Widget.
        float m_height;                     // Height of the Widget.
        unsigned int m_zOffset;             // The z-Depth of this widget
        bool m_isInteractable;
        bool m_isActive;

    public: 
        Widget(const WidgetConstructionData& data);
        virtual ~Widget() = default;

        void SetParent(Widget* pParent);
        virtual void AddChild(Widget* pChild); // Should this handle the creation of the widget and return the child?
        virtual void RemoveChild(Widget* pChild);
        void DestroyWidget();
        void DestroyWidgetAndChildren();
        virtual bool Init() { return true; }
        void Focus();

        // Event Handling
        void OnEvent(ApplicationEvent& event);

        // Setters
        virtual void SetPositionOffset(const float x, const float y);
        void SetAnchor(const float x, const float y);
        void SetZOffset(const unsigned int zOffset);
        virtual void SetActive(const bool isActive);
        void SetUILayer(UILayer* pUILayer) { m_pUILayer = pUILayer; }

        // Getters
        [[nodiscard]] RectF GetScreenRect() const;
        [[nodiscard]] unsigned int GetZOrder() const;
        [[nodiscard]] bool HasChildren() const { return !m_children.empty(); }
        [[nodiscard]] bool IsInteractable() const { return m_isInteractable; }
        [[nodiscard]] bool IsActive() const;
        [[nodiscard]] virtual WidgetTypeId GetTypeId() const = 0;

    protected:
        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //		
        ///		@brief : Virtual function to override how this Widget handles Application Events. The default behavior is to do nothing.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void HandleEvent([[maybe_unused]] ApplicationEvent& event) {}
        virtual void OnParentActiveChanged(const bool parentActiveState) {}

        [[nodiscard]] bool PointIntersectsRect(const Vec2 screenPos) const;
    };
}
