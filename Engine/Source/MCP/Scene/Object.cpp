// Object.cpp

#include "Object.h"

#include "MCP/Scene/Scene.h"

namespace mcp
{
    Object::Object(Scene* pScene)
        : m_pScene(pScene)
        , m_objectId(s_idCounter++)
        , m_isActive(true)
        , m_isQueuedForDeletion(false)
    {
        //
    }

    Object::~Object()
    {
        // Delete the components in reverse order, due to component dependencies.
        for (size_t i = 0; i < m_components.size(); ++i)
        {
            auto* pComponent = m_components[m_components.size() - 1 - i];
            BLEACH_DELETE(pComponent);
            pComponent = nullptr;
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Initialization step when the scene has finished loading. Used to 
    ///		@returns : 
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Object::PostLoadInit() const
    {
        for (auto* pComponent : m_components)
        {
            if (!pComponent->PostLoadInit())
            {
                MCP_ERROR("Object", "Object failed post Init!");
                return false;
            }
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Queues the destruction of this Object. This will in turn call 'OnDestroy' for each component.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Object::Destroy()
    {
        // If we are already queued, don't do anything.
        if (m_isQueuedForDeletion)
            return;

        // Queue the deletion.
        m_isQueuedForDeletion = true;
        GetWorld()->DestroyObject(m_objectId);

        // Signal to the components that we are being destroyed.
        for (auto* pComponent : m_components)
        {
            pComponent->OnDestroy();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Add a previously created Component to this Object.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Object::AddComponent(Component* pComponent)
    {
        if (!pComponent)
        {
            MCP_WARN("Object", "Attempted to add Component* that was nullptr!");
            return;
        }

        // Set the owner directly.
        pComponent->m_pOwner = this;
        pComponent->Init();

        // Add the Component to our list.
        m_components.emplace_back(pComponent);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Should the components' Active state be set to match the GameObject?
    //          - No. I think the better option is to have Components check to see if their Owner is Active...hmm...
    //
    ///		@brief : Set whether the GameObject is Active or not.
    ///		@param isActive : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void Object::SetActive(const bool isActive)
    {
        if (m_isActive == isActive)
            return;

        m_isActive = isActive;

        // Update our component's active state
        for (auto* pComponent : m_components)
        {
            pComponent->OnObjectActiveChanged(m_isActive);
        }
    }

    WorldLayer* Object::GetWorld() const
    {
        return m_pScene->GetWorldLayer();
    }
}
