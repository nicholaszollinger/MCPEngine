// UILayer.cpp

#include "UILayer.h"

#include "MCP/Core/Event/ApplicationEvent.h"
#include "MCP/Scene/Scene.h"

#include "MCP/UI/Widget.h"
#include "MCP/UI/CanvasWidget.h"
#include "MCP/UI/ImageWidget.h"
#include "MCP/UI/SelectionWidget.h"
#include "MCP/UI/SliderWidget.h"
#include "MCP/UI/TextWidget.h"
#include "MCP/UI/ToggleWidget.h"

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

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Load the UILayer
    //-----------------------------------------------------------------------------------------------------------------------------
    bool UILayer::LoadLayer(const XMLElement layer)
    {
        XMLElement element = layer.GetChildElement();

        while(element.IsValid())
        {
            if (HashString32(element.GetName()) == kSceneLayerAssetId)
            {
                LoadSceneDataAsset(element);
            }

            else
            {
                LoadWidget(element);   
            }

            element = element.GetSiblingElement();
        }

        //DumpUITree();

        return true;
    }

    bool UILayer::OnSceneLoad()
    {
        for (auto* pWidget : m_widgets)
        {
            // Initialize all of our children,
            pWidget->ForAllChildren([](Widget* pWidget)
            {
                pWidget->PostLoadInit();
            });

            // Initialize the parent.
            pWidget->PostLoadInit();
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Load a widget and all of its children from data.
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::LoadWidget(XMLElement rootElement)
    {
        while (rootElement.IsValid())
        {
            // Create a new widget in the context of this layer.
                // I need to add layer context somehow.
            const char* widgetTypename = rootElement.GetAttributeValue<const char*>("type");

            auto* pWidget = WidgetFactory::CreateWidgetFromData(widgetTypename, rootElement);
            pWidget->SetUILayer(this);
            pWidget->Init();

            // Recursively load each child widget.
            LoadChildWidget(pWidget, rootElement);

            // Add the parent widget to the UILayer.
            AddWidget(pWidget);

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
            const char* widgetTypename = childWidgetElement.GetAttributeValue<const char*>("type");

            Widget* pChild = WidgetFactory::CreateWidgetFromData(widgetTypename, childWidgetElement);
            pChild->SetUILayer(this);
            pChild->Init();
            pChild->SetParent(pParent);
            // Recursively get the Children of this child.
            LoadChildWidget(pChild, childWidgetElement);

            // Get the next child.
            childWidgetElement = childWidgetElement.GetSiblingElement("Widget");
        }
    }

    void UILayer::LoadSceneDataAsset(const XMLElement sceneDataAsset)
    {
        const char* pPath = sceneDataAsset.GetAttributeValue<const char*>("path");
        MCP_CHECK_MSG(pPath, "Failed to load SceneDataAsset on the UI Layer! No path was found!");

        XMLParser parser;
        if (!parser.LoadFile(pPath))
        {
            MCP_ERROR("UILayer", "Failed to load SceneDataAsset at path: ", pPath);
            return;
        }

        XMLElement assetElement = parser.GetElement();
        while(assetElement.IsValid())
        {
            if (HashString32(assetElement.GetName()) == kSceneLayerAssetId)
            {
                LoadSceneDataAsset(assetElement);
            }

            else
            {
                LoadWidget(assetElement);   
            }

            assetElement = assetElement.GetSiblingElement();
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
        // If we are trying to set an inactive widget as focused, return.
        if (pWidget && !pWidget->IsActive())
        {
            MCP_WARN("UILayer", "Failed to focus Widget. Tried to set an inactive widget as focused. Make sure to set it to active before focusing.");
            return;
        }

        //MCP_LOG("UILayer", "Setting FocusedWidget: ", *pWidget->GetTag());
        m_pFocusedWidget = pWidget;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Creates a new Widget from data. This function does NOT add the returned widget to the array of 'root' widgets.
    ///             if you want to add it afterwards, call AddWidget(). This function assumes that we are loading a single widget and
    ///             its children from data; it will NOT check for sibling widgets.
    ///		@param root : Root element for the widget we are loading.
    ///		@returns : Nullptr if there was an error, otherwise, the new Widget.
    //-----------------------------------------------------------------------------------------------------------------------------
    Widget* UILayer::CreateWidgetFromData(const XMLElement root)
    {
        if (!root.IsValid())
        {
            MCP_ERROR("UILayer", "Failed to Add Widget from data! Root element was invalid!");
            return nullptr;
        }

        const char* widgetTypename = root.GetAttributeValue<const char*>("type");

        auto* pWidget = WidgetFactory::CreateWidgetFromData(widgetTypename, root);
        pWidget->SetUILayer(this);
        pWidget->Init();

        // Recursively load each child widget.
        LoadChildWidget(pWidget, root);

        return pWidget;
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

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Debug function to visualize the UI tree
    //-----------------------------------------------------------------------------------------------------------------------------
    void UILayer::DumpUITree()
    {
        for (auto* pWidget : m_widgets)
        {
            PrintWidgetType(pWidget, 0);
        }
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
