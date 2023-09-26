// Component.cpp

#include "Component.h"

#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    Component::Component(Object* pObject)
        : m_pOwner(pObject)
        , m_isActive(true)
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
}
