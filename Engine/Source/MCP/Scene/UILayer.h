#pragma once
// UILayer.h

#include "SceneLayer.h"
#include "MCP/UI/Widget.h"

namespace mcp
{
    class UILayer final : public SceneLayer
    {
        MCP_DECLARE_LAYER_ENTITY_TYPE(Widget)

        struct WidgetStackElement
        {
            Widget* pBaseWidget = nullptr;          // The BaseWidget for this stack element. Ex: The base CanvasWidget of a Menu.
            Widget* pPreviouslyFocused = nullptr;   // The Widget on the previous menu that was focused. This can be nullptr.
        };

        std::vector<WidgetStackElement> m_stack;    // Stack of Widgets that define Menu navigation, render order, etc.
        Widget* m_pFocusedWidget;                   // The active Widget tree that is responding to ApplicationEvents

#if MCP_EDITOR
        Widget* m_pSelectedWidget = nullptr;
#endif

    public:
        // Entity Management
        template<typename WidgetType, typename...CArgs>
        WidgetType* CreateWidgetOfType(CArgs&&...args);

        virtual Widget* CreateEntityFromPrefab(const XMLElement root) override;
        template<typename WidgetType> WidgetType* GetWidgetByTag(const StringId tag);
        [[nodiscard]] Widget* GetWidgetByTag(const StringId tag) const;

        // UI Stack
        void AddToStack(Widget* pWidget);
        void PopStack();

        void FocusWidget(Widget* pWidget);
        [[nodiscard]] Widget* GetFocused() const { return m_pFocusedWidget; }

#if MCP_EDITOR
        void SetSelected(Widget* pWidget) { m_pSelectedWidget = pWidget; }
#endif

    private:
        // Private Ctor
        UILayer(Scene* pScene);

        // Scene Layer Loop
        virtual bool OnSceneLoad() override;
        virtual void Update(const float deltaTimeMs) override;
        virtual void FixedUpdate(const float fixedUpdateTimeS) override;
        virtual void Render() override;
        virtual void OnEvent(ApplicationEvent& event) override;

        // Entity Management
        virtual Widget* CreateEntity() override;
        virtual void LoadEntity(XMLElement element) override;
        void LoadChildWidget(Widget* pParent, XMLElement parentElement);
    };

    template <typename WidgetType, typename... CArgs>
    WidgetType* UILayer::CreateWidgetOfType(CArgs&&...args)
    {
        if (WidgetType::GetStaticLayerId() != GetLayerId())
        {
            MCP_WARN("UILayer", "Attempted to create a type that does not belong on the UILayer!");
            return nullptr;
        }


        auto* pWidget = BLEACH_NEW(WidgetType(std::forward<CArgs>(args)...));

        // Add the Entity to this layer.
        m_entities.emplace(pWidget->GetId(), pWidget);

        // Set their layer.
        pWidget->SetLayer(this);

        // Initialize the Widget
        pWidget->Init();

        return pWidget;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Searches *all* of the widgets in the Scene by type and tag. If no Widget matching the criteria was found, then
    ///         this returns nullptr.
    ///		@tparam WidgetType : Type of widget you are trying to find.
    ///		@param tag : Tag given to the Widget you are looking for.
    ///		@returns : Pointer to the Widget if we found it, otherwise nullptr.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename WidgetType>
    WidgetType* UILayer::GetWidgetByTag(const StringId tag)
    {
        for (auto&[id, pEntity] : m_entities)
        {
            auto* pWidget = SceneEntity::SafeCastEntity<Widget>(pEntity);

            // Check the parent
            if (pWidget->GetTypeId() == WidgetType::GetStaticTypeId() && tag == pWidget->GetTag())
            {
                return static_cast<WidgetType*>(pWidget);
            }

            // Check all of the children of this widget.
            if (auto* pResult = pWidget->FindChildByTag<WidgetType>(tag))
                return pResult;
        }

        return nullptr;
    }
}
