// Component.cpp

#include "Component.h"

#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    Component::Component(const bool startActive)
        : m_pOwner(nullptr)
        , m_isActive(startActive)
    {
        //
    }

    Component::Component(const ComponentConstructionData& data)
        : m_pOwner(nullptr)
        , m_isActive(data.startActive)
    {
        //
    }

    MessageManager* Component::GetMessageManager() const
    {
        return m_pOwner->GetScene()->GetMessageManager();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set this component's active state.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Component::SetActive(const bool isActive)
    {
        if (m_isActive == isActive)
            return;

        m_isActive = isActive;

        m_isActive ? OnActive() : OnInactive();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Return whether the Component is active. Note, if the Component is active but the owner is not, this will return
    ///         false.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Component::IsActive() const
    {
        if (!m_pOwner->IsActive())
            return false;

        return m_isActive;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Send out a message.
    ///		@param messageId : Name of the message type you want to send.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Component::SendMessage(const MessageId messageId)
    {
        m_pOwner->GetScene()->GetMessageManager()->QueueMessage(this, messageId);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set this component to listen for a specific type of message.
    ///		@param messageId : Name of the message that you want to listen to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Component::ListenForMessage(const MessageId messageId)
    {
        m_pOwner->GetScene()->GetMessageManager()->AddListener(this, messageId);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Stop listening to a specific message.
    ///		@param messageId : Name of the message that you want to stop listening to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Component::StopListeningToMessage(const MessageId messageId) const
    {
        m_pOwner->GetScene()->GetMessageManager()->RemoveListener(this, messageId);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Component response to the Object's active state changing.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Component::OnOwnerActiveChanged(const bool objectActive)
    {
        // If we were active, then we need to respond to the active change.
        if (m_isActive)
        {
            if (objectActive)
            {
                OnActive();
            }

            else
            {
                OnInactive();
            }
        }
    }

    ComponentConstructionData Component::GetComponentConstructionData(const XMLElement element)
    {
        ComponentConstructionData data;
        data.startActive = element.GetAttributeValue<bool>("startActive", true);
        return data;
    }
}
