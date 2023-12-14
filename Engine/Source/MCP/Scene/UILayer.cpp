// UILayer.cpp

#include "UILayer.h"

#include "LuaSource.h"
#include "SceneAsset.h"
#include "MCP/Core/Event/ApplicationEvent.h"
#include "MCP/UI/Widget.h"
#include "MCP/UI/CanvasWidget.h"
#include "MCP/UI/ImageWidget.h"
#include "MCP/UI/SelectionWidget.h"
#include "MCP/UI/SliderWidget.h"
#include "MCP/UI/TextWidget.h"
#include "MCP/UI/ToggleWidget.h"
#include "MCP/Scene/Scene.h"

#if MCP_EDITOR
#include "MCP/Core/Application/Application.h"
#endif

namespace mcp
{
    UILayer::UILayer(Scene* pScene)
        : SceneLayer(pScene)
        , m_pFocusedWidget(nullptr)
    {
        //
    }

    bool UILayer::OnSceneLoad()
    {
        for (auto&[id, pEntity] : m_entities)
        {
            auto* pWidget = SceneEntity::SafeCastEntity<Widget>(pEntity);

            if (pWidget->HasAParent())
                continue;

            // Initialize all of our children,
            pWidget->ForAllChildren([](Widget* pWidget)
            {
                pWidget->PostLoadInit();
            });

            // Initialize the parent.
            pWidget->PostLoadInit();
        }

        m_state = LayerState::kPostLoad;

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Adds the Widget onto our current UIStack, and sets it active. NOTE: This does not create a widget, it just adds
    ///             the widget to our current stack.
    ///		@param pWidget : Widget that you are adding. This Widget should already be loaded in memory.
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::AddToStack(Widget* pWidget)
    {
        if (!pWidget)
        {
            MCP_WARN("UI", "Failed to open menu! pWidget was nullptr!");
            return;
        }

        // Set the new zOffset of the widget:
        // Get the current Z value of our widget stack.
        auto currentMaxZ = 0;
        if (!m_stack.empty())
            currentMaxZ = m_stack.back().pBaseWidget->GetMaxZ() + 1;

        pWidget->SetZOffset(currentMaxZ);

        // Add the to the top of the stack and set it active.
        // Activate the new top Widget
        m_stack.emplace_back(WidgetStackElement{pWidget, m_pFocusedWidget});
        pWidget->SetActive(true);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Closes the top Widget on the stack, and if valid, it will return focus to the Widget that was previously focused.
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::PopStack()
    {
        if (m_stack.empty())
            return;

        // Disable the top menu
        auto& pTop = m_stack.back();
        pTop.pBaseWidget->SetActive(false);

        // Refocus the last focused widget.
        FocusWidget(pTop.pPreviouslyFocused);

        // Remove it from the stack.
        m_stack.pop_back();
    }

    Widget* UILayer::CreateEntity()
    {
        MCP_CHECK(false);
        return nullptr;
    }

    Widget* UILayer::CreateEntityFromPrefab(const XMLElement root)
    {
        if (!root.IsValid())
        {
            MCP_ERROR("UILayer", "Failed to Add Widget from data! Root element was invalid!");
            return nullptr;
        }

#ifndef _DEBUG
        if (AssetIsDebugOnly(root))
        {
            return nullptr;
        }
#endif

        const char* widgetTypename = root.GetAttributeValue<const char*>("type");

        auto* pWidget = WidgetFactory::CreateWidgetFromData(widgetTypename, root);
        pWidget->SetLayer(this);
        pWidget->Init();

        // Recursively load each child widget.
        LoadChildWidget(pWidget, root);

        // Add the Widget to our Layer.
        AddEntity(pWidget);

        return pWidget;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Load a widget and all of its children from data.
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::LoadEntity(XMLElement rootElement)
    {
        while (rootElement.IsValid())
        {
#ifndef _DEBUG
            if (AssetIsDebugOnly(rootElement))
            {
                rootElement = rootElement.GetSiblingElement("Widget");
                continue;
            }
#endif

            // Create a new widget in the context of this layer.
                // I need to add layer context somehow.
            const char* widgetTypename = rootElement.GetAttributeValue<const char*>("type");

            auto* pWidget = WidgetFactory::CreateWidgetFromData(widgetTypename, rootElement);

            // Add the parent widget to the UILayer.
            AddEntity(pWidget);

            // Initialize
            pWidget->Init();

#if MCP_EDITOR
            // HACK FOR THE ASSIGNMENT - REMOVE THIS LATER:
            // Set the root to active if we are being opening in the 'tool' version of the app.
            if (Application::Get()->GetContext().args.count > 1)
            {
                pWidget->SetActive(true);
                //m_pFocusedWidget = pWidget;
            }
            
#endif

            // Recursively load each child widget.
            LoadChildWidget(pWidget, rootElement);

            // If the parent widget is enabled, then we will add it to the UI Stack.
            // This means that the order of the loading is potentially something to worry about...
            if (pWidget->IsActive())
                AddToStack(pWidget);

            // Get the next root Widget element.
            rootElement = rootElement.GetSiblingElement("Widget");
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Recursively loads the children of a parent Widget.
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::LoadChildWidget(Widget* pParent, XMLElement parentElement)
    {
        XMLElement childWidgetElement = parentElement.GetChildElement("Widget");
        while(childWidgetElement.IsValid())
        {
#ifndef _DEBUG
            if (AssetIsDebugOnly(childWidgetElement))
            {
                childWidgetElement = childWidgetElement.GetSiblingElement("Widget");
                continue;
            }
#endif

            const char* widgetTypename = childWidgetElement.GetAttributeValue<const char*>("type");

            Widget* pChild = WidgetFactory::CreateWidgetFromData(widgetTypename, childWidgetElement);
            if (!pChild)
            {
                childWidgetElement = childWidgetElement.GetSiblingElement("Widget");
                continue;
            }

            // Add the child to our Layer
            AddEntity(pChild);

            // Initialize
            pChild->Init();

            // Set the Parent.
            pChild->SetParent(pParent);

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

#if MCP_EDITOR
        // If we have a selected Widget, render its bounding box.
        if (m_pSelectedWidget)
        {
            const auto rect = m_pSelectedWidget->GetRectTopLeft();
            DrawRect(rect, Color::White());
        }

#endif
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
#if MCP_EDITOR
        if (event.GetEventId() != MouseButtonEvent::GetStaticId())
            return;

        auto& mouseEvent = static_cast<MouseButtonEvent&>(event);

         // If there is no focused Widget and we are clicking on the screen, attempt to find a widget.
        if (!m_pFocusedWidget) // && event.GetEventId() == MouseButtonEvent::GetStaticId())
        {
            if (mouseEvent.State() != ButtonState::kPressed)
                return;

            for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
            {
                if (it->pBaseWidget->PointIntersectsRect(mouseEvent.GetWindowPosition()))
                {
                    m_pFocusedWidget = it->pBaseWidget;

                    //const auto tag = m_pFocusedWidget->GetTag();
                    //MCP_LOG("UILayer", "Focused: ", *tag);
                    break;
                }
            }
        }

        // Send the event down to the widgets.
        if (m_pFocusedWidget)
            m_pFocusedWidget->OnEvent(event);

#else

        // If there is no focused Widget, then return
        if (!m_pFocusedWidget)
            return;

        // Send the event down to the widgets.
        m_pFocusedWidget->OnEvent(event);
#endif
    }
    
    Widget* UILayer::GetWidgetByTag(const StringId tag) const
    {
        for (auto&[id, pEntity] : m_entities)
        {
            auto* pWidget = SceneEntity::SafeCastEntity<Widget>(pEntity);

            // If this isn't a root, don't search it.
            if (pWidget->HasAParent())
                continue;

            if (pWidget->GetTag() == tag)
            {
                return pWidget;
            }

            // Search the children, and if we found the Widget, return it.
            if (auto* pFound = pWidget->GetChildByTag(tag))
                return pFound;
        }

        return nullptr;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set a Widget as focused.
    ///		@param pWidget : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::FocusWidget(Widget* pWidget)
    {
        // If we are trying to set an inactive widget as focused, return.
        if (pWidget && !pWidget->IsActive())
        {
            MCP_WARN("UILayer", "Failed to focus Widget. Tried to set an inactive widget as focused. Make sure to set it to active before focusing.");
            return;
        }

        // If we had a previously focused widget, have it respond to the change.
        if (m_pFocusedWidget)
            m_pFocusedWidget->OnFocusChanged(false);

        //MCP_LOG("UILayer", "Setting FocusedWidget: ", *pWidget->GetTag());
        m_pFocusedWidget = pWidget;

        // If our widget is not nullptr, have it respond to the change.
        if (m_pFocusedWidget)
            m_pFocusedWidget->OnFocusChanged(true);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Debug function to visualize the UI tree
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::DumpUITree()
    {
        /*for (auto* pWidget : m_widgets)
        {
            PrintWidgetType(pWidget, 0);
        }*/
    }

    void UILayer::PrintWidgetType(Widget* pWidget, int tabCount)
    {
        std::string line = "";
        for (int i = 0; i < tabCount; ++i)
        {
            line += '\t';
        }

        const auto id = pWidget->GetTypeId();

        if (id == CanvasWidget::GetStaticTypeId())
        {
            MCP_LOG("UILayer", line, "CanvasWidget: ", *pWidget->GetTag());
        }

        else if (id == ImageWidget::GetStaticTypeId())
        {
            MCP_LOG("UILayer", line, "ImageWidget");
        }

        else if (id == ButtonWidget::GetStaticTypeId())
        {
            MCP_LOG("UILayer", line, "ButtonWidget");
        }

        else if (id == ToggleWidget::GetStaticTypeId())
        {
            MCP_LOG("UILayer", line, "ToggleWidget");
        }

        else if (id == SliderWidget::GetStaticTypeId())
        {
            MCP_LOG("UILayer", line, "SliderWidget");
        }

        else if (id == TextWidget::GetStaticTypeId())
        {
            MCP_LOG("UILayer", line, "TextWidget");
        }

        else if (id == SelectionWidget::GetStaticTypeId())
        {
            MCP_LOG("UILayer", line, "SelectionWidget");
        }

        else
        {
            MCP_LOG("UILayer", line, "Uknown Widget");
        }

        if (pWidget->HasChildren())
        {
            ++tabCount;
            pWidget->ForAllChildren([this, tabCount](Widget* pWidget){ this->PrintWidgetType(pWidget, tabCount); });
        }
    }
}
