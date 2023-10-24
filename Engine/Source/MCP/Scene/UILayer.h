#pragma once
// UILayer.h

#include "SceneLayer.h"
#include "MCP/UI/Widget.h"

namespace mcp
{
    class UILayer final : public SceneLayer
    {
        std::vector<Widget*> m_queuedWidgetsToDelete;
        std::vector<Widget*> m_widgets;                // Container of all Widgets in the scene. More specifically, these are all of the 'roots'
        Widget* m_pFocusedWidget;                      // The active Widget tree that is responding to ApplicationEvents

    public:
        UILayer(Scene* pScene);
        virtual ~UILayer() override;

        virtual bool OnSceneLoad() override;
        virtual bool LoadLayer(const XMLElement layer) override;
        virtual void Update(const float deltaTimeMs) override;
        virtual void FixedUpdate(const float fixedUpdateTimeS) override;
        virtual void Render() override;
        virtual void OnEvent(ApplicationEvent& event) override;

        template<typename WidgetType>
        WidgetType* GetWidgetByTag(const StringId tag);

        void AddWidget(Widget* pWidget);
        Widget* CreateWidgetFromData(const XMLElement root);
        void FocusWidget(Widget* pWidget);
        void DeleteWidget(Widget* pWidget);

        [[nodiscard]] Widget* GetFocused() const { return m_pFocusedWidget; }

    private:
        virtual void LoadSceneDataAsset(const XMLElement sceneDataAsset) override;
        virtual void DestroyLayer() override;
        void LoadWidget(XMLElement rootElement);
        void LoadChildWidget(Widget* pParent, XMLElement parentElement);
        void DeleteQueuedWidgets();

        void DumpUITree();
        void PrintWidgetType(Widget* pWidget, int tabCount);
    };

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
        for (auto* pWidget : m_widgets)
        {
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
