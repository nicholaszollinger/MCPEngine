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
                LogError("Object failed post Init!");
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
        m_pScene->DestroyObject(m_objectId);

        // Signal to the components that we are being destroyed.
        for (auto* pComponent : m_components)
        {
            pComponent->OnDestroy();
        }
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
        m_isActive = isActive;

        /*for (auto* pComponent : m_components)
        {
            pComponent->SetIsActive(isActive);
        }*/
    }
}
