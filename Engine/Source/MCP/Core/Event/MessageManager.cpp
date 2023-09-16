// MessageManager.cpp

#include "MessageManager.h"
#include "MCP/Components/Component.h"

namespace mcp
{
    void MessageManager::ProcessMessages()
    {
        // Go through the queue and dispatch the events.
        for (auto& message : m_messageQueue)
        {
            // TODO: Make sure the message.pSender is still valid? Should that matter?

            // If we have listeners for that message type, send the message to the receivers.
            if (const auto result = m_messageReceivers.find(message.id); result != m_messageReceivers.end())
            {
                for (auto* pComponent : result->second)
                {
                    pComponent->ReceiveMessage(message);
                }
            }
        }

        // Clear out the queue.
        m_messageQueue.clear();
    }

    void MessageManager::AddReceiver(Component* pReceiver, const MessageIdType id)
    {
        m_messageReceivers[id].emplace_back(pReceiver);
    }

    void MessageManager::RemoveReceiver(const Component* pReceiver, const MessageIdType id)
    {
        // Check to see if we have any receivers for that type.
        if (const auto result = m_messageReceivers.find(id); result != m_messageReceivers.end())
        {
            // This is O(n) but I don't imagine these arrays will be huge. But who knows. Might have to
            // change later.
            for (size_t i = 0; i < result->second.size(); ++i)
            {
                if (result->second[i] == pReceiver)
                {
                    // Swap and pop.
                    std::swap(result->second[i], result->second.back());
                    result->second.pop_back();

                    break;
                }
            }
        }
    }

    void MessageManager::QueueMessage(Component* pSender, const MessageIdType id)
    {
        m_messageQueue.emplace_back(id, pSender);
    }
}