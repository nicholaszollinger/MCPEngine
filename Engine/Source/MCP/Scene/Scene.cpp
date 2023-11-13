// Scene.cpp

#include "Scene.h"

#include <algorithm>
#include "IRenderable.h"
#include "MCP/Components/ComponentFactory.h"
#include "MCP/Components/TransformComponent.h"
#include "MCP/Core/Application/Application.h"
#include "MCP/Core/Event/ApplicationEvent.h"
#include "MCP/Core/Event/Event.h"
#include "MCP/Core/Resource/PackageManager.h"
#include "MCP/Core/Resource/Parser.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Scene/IUpdateable.h"
#include "MCP/Scene/Object.h"

namespace mcp
{
    Scene::Scene()
        : m_pWorldLayer(nullptr)
        , m_pUILayer(nullptr)
        , m_accumulatedTime(0.f)
        , m_transitionQueued(false)
        , m_isLoaded(false)
    {
        //
    }

    Scene::~Scene()
    {
        if (m_isLoaded)
            Destroy();
    }

    bool Scene::Load(const char* pFilePath)
    {
        XMLParser parser;
        if (!parser.LoadFile(pFilePath))
        {
            MCP_ERROR("Scene", "Failed to load Scene!");
            return false;
        }

        // Check to see if we need to load any packages for this Scene
        const XMLElement packageElement = parser.GetElement(kPackageElementName);
        if (packageElement.IsValid())
        {
            // Get the Package file path:
            const char* pPackageFilePath = packageElement.GetAttributeValue<const char*>("path");
            if (!pPackageFilePath)
            {
                MCP_ERROR("Scene", "Failed to load scene! Couldn't find Package path attribute in xml file!");
                return false;
            }

            // Load the package.
            if (!PackageManager::Get()->LoadPackage(pPackageFilePath))
            {
                MCP_ERROR("Scene", "Failed to load scene! Couldn't load package defined in xml 'path' attribute. Path: ", pPackageFilePath);
                return false;
            }
        }

        // Get the Scene Element.
        const XMLElement sceneElement = parser.GetElement(kSceneElementName);
        if (!sceneElement.IsValid())
        {
            // Oh no the formatting of the xml document isn't standardized!
            MCP_ERROR("Scene", "Failed to load scene! Couldn't find Scene Element!");
            return false;
        }

        // Load the UI Layer:
        XMLElement sceneLayer = sceneElement.GetChildElement("UI");
        MCP_CHECK(sceneLayer.IsValid());

        m_pUILayer = BLEACH_NEW(UILayer(this));
        if (!m_pUILayer->LoadLayer(sceneLayer))
        {
            MCP_ERROR("Scene", "Failed to load UI Layer");
            Destroy();
            return false;
        }

        // Load the World Layer
        sceneLayer = sceneLayer.GetSiblingElement("World");
        MCP_CHECK(sceneLayer.IsValid());

        m_pWorldLayer = BLEACH_NEW(WorldLayer(this));
        if (!m_pWorldLayer->LoadLayer(sceneLayer))
        {
            MCP_ERROR("Scene", "Failed to load World layer!");
            Destroy();
            return false;
        }

        // Initialize the Scene.
        m_isLoaded = Init();

        return m_isLoaded;
    }

    bool Scene::Init()
    {
        MCP_ADD_MEMBER_FUNC_EVENT_LISTENER(ApplicationEvent, OnEvent);

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : OnSceneLoad is called once the scene has completed loading. Each layer is then given an opportunity to do
    ///         any initialization of their scene entities.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Scene::OnSceneLoad() 
    {
        if (!m_pWorldLayer->OnSceneLoad())
        {
            MCP_ERROR("Scene", "World layer failed OnSceneLoad!");
            return false;
        }

        if (!m_pUILayer->OnSceneLoad())
        {
            MCP_ERROR("Scene", "UI Layer failed OnSceneLoad!");
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      TODO: We should time slice this!
    //
    ///		@brief : Update each scene layer from bottom to top.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Scene::Update(const float deltaTime)
    {
        //m_messageManager.ProcessMessages();
        // Update the Layers.
        m_pWorldLayer->Update(deltaTime);
        m_pUILayer->Update(deltaTime);

        // If enough time has passed, perform a fixed update.
        m_accumulatedTime += deltaTime;
        if (m_accumulatedTime > kFixedUpdateTimeSeconds)
        {
            m_accumulatedTime -= kFixedUpdateTimeSeconds;
            m_pWorldLayer->FixedUpdate(kFixedUpdateTimeSeconds);
            m_pUILayer->FixedUpdate(kFixedUpdateTimeSeconds);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Render each scene layer from the bottom up.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Scene::Render() const
    {
        m_pWorldLayer->Render();
        m_pUILayer->Render();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Closes the scene, destroying each scene layer.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Scene::Destroy()
    {
        // Stop listening to ApplicationEvents.
        MCP_REMOVE_MEMBER_FUNC_EVENT_LISTENER(ApplicationEvent);

        m_pWorldLayer->DestroyLayer();
        m_pUILayer->DestroyLayer();

        // Destroy each layer
        BLEACH_DELETE(m_pWorldLayer);
        m_pWorldLayer = nullptr;

        BLEACH_DELETE(m_pUILayer);
        m_pUILayer = nullptr;

        m_isLoaded = false;
    }

    MessageManager* Scene::GetMessageManager()
    {
        return m_pWorldLayer->GetMessageManager();
    }

    SceneLayer* Scene::GetLayer(const LayerId id) const
    {
        // TODO: When/if I have layers in an array, this would just compare to each id in a loop.
        switch(id)
        {
            case WorldLayer::GetStaticLayerId(): return m_pWorldLayer;
            case UILayer::GetStaticLayerId(): return m_pUILayer;
            default: return nullptr;
        }
    }

    void Scene::OnEvent(ApplicationEvent& event) const
    {
        // For each layer in reverse, send the event. If the event is handled, then return.
        // Right now I have two hard coded layers, so we are going to do UI and then the World
        m_pUILayer->OnEvent(event);

        if (!m_isLoaded || event.IsHandled())
            return;

        m_pWorldLayer->OnEvent(event);
    }
}
