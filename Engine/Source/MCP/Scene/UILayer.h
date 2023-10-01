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

        virtual bool LoadLayer(const XMLElement layer) override;
        virtual void Update(const float deltaTimeMs) override;
        virtual void FixedUpdate(const float fixedUpdateTimeS) override;
        virtual void Render() override;
        virtual void OnEvent(ApplicationEvent& event) override;

        void AddWidget(Widget* pWidget);
        void FocusWidget(Widget* pWidget);
        void DeleteWidget(Widget* pWidget);

    private:
        void LoadChildWidget(Widget* pParent, XMLElement parentElement);
        void DeleteQueuedWidgets();
        virtual void DestroyLayer() override;
    };
}
