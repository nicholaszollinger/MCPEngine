// Message.cpp

#include "Message.h"

#include "MCP/Components/Component.h"
#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    Messenger::Messenger(const MessageIdType id, Component* pOwner)
        : m_pOwner(pOwner)
        , m_id(id)
    {
        //
    }

    Messenger::~Messenger()
    {
        auto* pMessageManager = m_pOwner->GetMessageManager();
        assert(pMessageManager);
        pMessageManager->RemoveReceiver(m_pOwner, m_id);
    }

    void Messenger::SendMessage() const
    {
        auto* pMessageManager = m_pOwner->GetMessageManager();
        assert(pMessageManager);
        pMessageManager->QueueMessage(m_pOwner, m_id);
    }
}