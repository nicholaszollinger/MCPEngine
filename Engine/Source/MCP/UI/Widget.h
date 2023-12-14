#pragma once
// WidgetComponent.h

#include <vector>

#include "WidgetFactory.h"
#include "MCP/Scene/SceneEntity.h"
#include "MCP/Scripting/Script.h"
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
        SceneEntityConstructionData entityConstructionData;
        RectF rect {};                           // Position of the rect is the offset from the parent, or the actual screen position if the parent is nullptr.
        Vec2 anchor = {0.5f, 0.5f};         // How the Widget is anchored to it's parent. Default is to anchor to the center.
        Vec2 scale = {1.f, 1.f};            // Scale of the Widget.
        Vec2 pivot = {0.5f, 0.5f};          // Position of the Widget in relation to its dimensions. Default is an origin at the center of the Rect.
        StringId tag = {};                       // Optional tag to give the Widget.
        LuaResourcePtr enableBehaviorScript;     // Optional script that defines extra behavior for enabling and disabling this widget.
        int zOffset = 1;                         // z-Axis difference from the parent.
        bool isInteractable = false;             // Whether this Widget is interactable or not.
        bool sizedToContent = false;             // Whether the Widget's dimensions is based on the size of its content.
        bool isMask = false;                     // Whether the rect is used as a mask for child widgets.
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Base class for any UI Components.
    //-----------------------------------------------------------------------------------------------------------------------------
    class Widget : public SceneEntity
    {
        friend class LuaSystem;
        MCP_DEFINE_SCENE_ENTITY(Widget)

    protected:
        Script m_enableBehaviorScript;  // Optional Script that defines behavior when this script is enabled or disabled.
        Widget* m_pMaskingWidget;                        // If a parent has been set as a mask, this will point to that parent Widget. We use this to calculate our final dimensions.
        Vec2 m_localPos;                        // Offset from the origin position
#if MCP_EDITOR
        Vec2 m_grabbedOffset;                   // The offset the mouse has clicked this widget from the origin.
        bool m_held;
#endif
        Vec2 m_pivot;                           // The alignment of the center point of the Widget.
        Vec2 m_anchor;                          // Determines the position of Widget based on the Parent's position. An anchor can have values from [0, 1]
        Vec2 m_scale;                           // Scale of the two axes.
        float m_width;                          // Width of the Widget.
        float m_height;                         // Height of the Widget.
        int m_zOffset;                          // The z-Depth of this widget
        bool m_isInteractable;                  // Whether this Widget able to be interacted with or not. Ex: Can I hit this button?
        bool m_isVisible;                       // Whether this Widget is being rendered or not.
        bool m_sizedToContent;                  // Whether the size of this widget is based on its content and its children.
        bool m_isMask;                          // Whether the rect is used as a mask for child widgets.

    public: 
        Widget(const WidgetConstructionData& data);
        virtual ~Widget() override = default;

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called right after construction, no parent or child widgets will set or created yet!
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual bool Init() override { return true; }

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called once the layer is loaded. All scene entities loaded from data have been created.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual bool PostLoadInit() override { return true; }
        
        void Focus();
        void OnFocusChanged(const bool isFocused);

        // Querying Children
        template<typename WidgetType> WidgetType* FindChildByTag(const StringId tag);
        template<typename WidgetType> WidgetType* FindFirstChildOfType();
        template<typename WidgetType> std::vector<WidgetType*> GetAllChildrenOfType();

        void ForAllChildren(const std::function<void(Widget* pWidget)>& task) const;
        [[nodiscard]] virtual Widget* GetChildByTag(const StringId tag) const override;
        [[nodiscard]] virtual Widget* GetChildByTag(const StringId tag) override;
        virtual void OnChildSizeChanged() {}

        // Event Handling
        void OnEvent(ApplicationEvent& event);

        // Setters
        virtual void SetLocalPosition(const float x, const float y);
        void SetPosition(const float x, const float y);
        void SetPosition(const Vec2 screenPos);
        void SetAnchor(const float x, const float y);
        void SetZOffset(const int zOffset);
        void SetInteractable(const bool isInteractable);
        void SetPivot(const Vec2 pivot);
        void SetPivot(float x, float y);
        void SetWidth(const float width);
        void SetHeight(const float height);
        void SetScale(const float xScale, const float yScale);
        void SetScale(const float uniformScale);
        void SetIsMask(const bool isMask);

#if MCP_EDITOR
        // Saving
        virtual void Save() override;
#endif

        // Getters
        [[nodiscard]] virtual float GetRectWidth() const;
        [[nodiscard]] virtual float GetRectHeight() const;
        [[nodiscard]] float GetMaxChildWidth() const;
        [[nodiscard]] float GetMaxChildHeight() const;
        [[nodiscard]] float GetTotalRectWidthOfChildren() const;
        [[nodiscard]] float GetTotalRectHeightOfChildren() const;
        [[nodiscard]] Vec2 GetOrigin() const;
        [[nodiscard]] Vec2 GetScreenSpacePos() const;
        [[nodiscard]] RectF GetRectTopLeft() const;
        [[nodiscard]] Vec2 GetRectCenter() const;
        [[nodiscard]] Vec2 GetLocalPosition() const { return m_localPos; }
        [[nodiscard]] Vec2 GetScale() const;
        [[nodiscard]] RectF GetRect() const;
        [[nodiscard]] RectF GetVisibleRect() const;
        [[nodiscard]] int GetMaxZ() const;
        [[nodiscard]] int GetZOffset() const;
        [[nodiscard]] bool PointIntersectsRect(const Vec2 screenPos) const;
        [[nodiscard]] bool IsMask() const;
        [[nodiscard]] bool IsClipped() const;
        [[nodiscard]] bool IsInteractable() const;
        [[nodiscard]] bool IsVisible() const;
        [[nodiscard]] bool IsFocused() const;
        [[nodiscard]] virtual WidgetTypeId GetTypeId() const = 0;
        [[nodiscard]] UILayer* GetUILayer() const;
        [[nodiscard]] virtual Widget* GetParent() const override { return SafeCastEntity<Widget>(m_pParent);}

    protected:
        static WidgetConstructionData GetWidgetConstructionData(const XMLElement widgetElement);
        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Virtual function to override how this Widget handles Application Events. The default behavior is to do nothing.
        ///         This function is only called if the Widget is active and is interactable.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void HandleEvent([[maybe_unused]] ApplicationEvent& event) {}

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when this Widget's m_isInteractable is set to true.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnEnable() {}

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when this Widget's m_isInteractable is set to false.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnDisable() {}

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when this Widget is Focused in the UILayer.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnFocus() {}

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Called when this Widget is removed as the focused Widget in the UILayer.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnFocusLost() {}
        
        virtual void OnActive() override;
        virtual void OnInactive() override;
        virtual void OnMove();
        virtual void OnParentSet() override;
        void UpdateMaskingWidget(Widget* pMaskingWidget);
        virtual void OnZChanged() {}
        virtual void OnDestroy() override {}

        virtual void OnChildAdded(SceneEntity* pChild) override;
        virtual void OnChildRemoved(SceneEntity* pChild) override;
        virtual void OnChildAdded([[maybe_unused]] Widget* pChild);
        virtual void OnChildRemoved([[maybe_unused]] Widget* pChild) {}
        [[nodiscard]] Widget* GetParentMask() const;
        [[nodiscard]] bool GetPointRelativeToRect(const Vec2 screenPos, Vec2& relativePos) const;

    private:
        void OnZSet();

        template<typename WidgetType> void AddAllChildrenOfType(std::vector<WidgetType*>& childrenOfType);

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
            auto* pWidgetChild = SafeCastEntity<Widget>(pChild);

            // If it matches this child, return the child casted to the correct type.
            if (pWidgetChild->GetTypeId() == WidgetType::GetStaticTypeId() && tag == pWidgetChild->GetTag())
            {
                return static_cast<WidgetType*>(pWidgetChild);
            }

            // Recursively check the children.
            if (WidgetType* pResult = pWidgetChild->FindChildByTag<WidgetType>(tag))
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
            auto* pWidgetChild = SafeCastEntity<Widget>(pChild);

            // If it matches this child, return the child casted to the correct type.
            if (pWidgetChild->GetTypeId() == WidgetType::GetStaticTypeId())
            {
                return static_cast<WidgetType*>(pWidgetChild);
            }

            // Recursively check the children.
            if (WidgetType* pResult = pWidgetChild->FindFirstChildOfType<WidgetType>())
                return pResult;
        }

        // If we didn't have a child by that tag of that type, return nullptr.
        return nullptr;
    }

    template <typename WidgetType>
    std::vector<WidgetType*> Widget::GetAllChildrenOfType()
    {
        std::vector<WidgetType*> widgets;

        AddAllChildrenOfType(widgets);

        return widgets;
    }

    template <typename WidgetType>
    void Widget::AddAllChildrenOfType(std::vector<WidgetType*>& childrenOfType)
    {
        for (auto* pChild : m_children)
        {
            auto* pWidgetChild = SafeCastEntity<Widget>(pChild);

            // If this child is the type, add it to the array
            if (pWidgetChild->GetTypeId() == WidgetType::GetStaticTypeId())
            {
                auto* pTypedWidget = static_cast<WidgetType*>(pWidgetChild);
                childrenOfType.emplace_back(pTypedWidget);
            }

            // Check the children of this Widget, recursively.
            pWidgetChild->AddAllChildrenOfType(childrenOfType);
        }
    }
}
