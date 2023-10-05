// UILayer.cpp

#include "UILayer.h"

#include "MCP/Core/Event/ApplicationEvent.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    UILayer::UILayer(Scene* pScene)
        : SceneLayer(pScene)
        , m_pFocusedWidget(nullptr)
    {
        //
    }

    UILayer::~UILayer()
    {
        DestroyLayer();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Add a widget to the Layer.
    ///		@param pWidget : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::AddWidget(Widget* pWidget)
    {
        m_widgets.push_back(pWidget);
    }

    bool UILayer::LoadLayer(const XMLElement layer)
    {
        // UI Objects
        XMLElement rootWidgetElement = layer.GetChildElement("Widget");
        while (rootWidgetElement.IsValid())
        {
            // Create a new widget in the context of this layer.
                // I need to add layer context somehow.
            const char* widgetTypename = rootWidgetElement.GetAttribute<const char*>("type");

            auto* pWidget = WidgetFactory::CreateWidgetFromData(widgetTypename, rootWidgetElement);
            pWidget->SetUILayer(this);
            pWidget->Init();

            // Recursively load each child widget.
            LoadChildWidget(pWidget, rootWidgetElement);

            // Add the parent widget to the UILayer.
            AddWidget(pWidget);

            // Get the next root Widget element.
            rootWidgetElement = rootWidgetElement.GetSiblingElement("Widget");
        }

        return true;
    }

    void UILayer::LoadChildWidget(Widget* pParent, XMLElement parentElement)
    {
        XMLElement childWidgetElement = parentElement.GetChildElement("Widget");
        while(childWidgetElement.IsValid())
        {
            const char* widgetTypename = childWidgetElement.GetAttribute<const char*>("type");

            Widget* pChild = WidgetFactory::CreateWidgetFromData(widgetTypename, childWidgetElement);
            pChild->SetParent(pParent);
            pChild->SetUILayer(this);
            pChild->Init();
            // Recursively get the Children of this child.
            LoadChildWidget(pChild, childWidgetElement);

            // Get the next child.
            childWidgetElement = childWidgetElement.GetSiblingElement("Widget");
        }
    }

    void UILayer::Update(const float deltaTimeMs)
    {
        const auto& updateableArray = m_updateables.GetArray();

        for (auto* pUpdateable : updateableArray)
        {
            pUpdateable->Update(deltaTimeMs);

            if (m_pScene->TransitionQueued())
                break;
        }
    }

    void UILayer::FixedUpdate(const float fixedUpdateTimeS)
    {
        const auto& updateableArray = m_fixedUpdateables.GetArray();

        for (auto* pUpdateable : updateableArray)
        {
            pUpdateable->Update(fixedUpdateTimeS);

            if (m_pScene->TransitionQueued()) // Get this from the Scene.
                break;
        }
    }

    void UILayer::Render()
    {
        static std::vector<IRenderable*> renderableArray;
        // This is a copy.....
        renderableArray = m_renderables.GetArray();

        // Sort the array based on the zOrder.
        std::sort(renderableArray.begin(), renderableArray.end(), [](const IRenderable* pLeft, const IRenderable* pRight)
        {
            return pLeft->GetZOrder() < pRight->GetZOrder();
        });

        // Render each renderable.
        for (const auto* pRenderable : renderableArray)
        {
            pRenderable->Render();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Right now, we are just going to respond to Mouse events. I need to be able to respond to Keyboard events if the keyboard
    //      has focus, and Window events like resizing.
    //		
    ///		@brief : Handle events sent from the application. Only the Widget that is currently focused will be responding Input events.
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::OnEvent(ApplicationEvent& event)
    {
        // If there is no focused Widget, then return
        if (!m_pFocusedWidget)
            return;

        // Send the event down to the widgets.
        m_pFocusedWidget->OnEvent(event);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set a Widget as focused.
    ///		@param pWidget : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::FocusWidget(Widget* pWidget)
    {
        m_pFocusedWidget = pWidget;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Queue the deletion of a Widget. NOTE: This should only be called by Widgets themselves so that they can properly
    ///         handle their relationships to other Widgets!
    ///		@param pWidget : Widget that is going to be deleted.
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::DeleteWidget(Widget* pWidget)
    {
        m_queuedWidgetsToDelete.push_back(pWidget);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Delete all of the Widgets that have been queued for deletion.
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::DeleteQueuedWidgets()
    {
        for (const auto pWidget : m_queuedWidgetsToDelete)
        {
            BLEACH_DELETE(pWidget);
        }

        m_queuedWidgetsToDelete.clear();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Destroys the Root and all of its children.
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::DestroyLayer()
    {
        // Destroy each 'root' that we have.
        for (auto* pWidget : m_widgets)
        {
            pWidget->DestroyWidgetAndChildren();
        }

        DeleteQueuedWidgets();
    }

}