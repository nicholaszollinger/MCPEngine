// MessageManager.cpp

#include "MessageManager.h"
#include "MCP/Components/Component.h"

namespace mcp
{
    void MessageManager::ProcessMessages()
    {
        // I make a copy of the queue so that if other messages are spawned as a result of a message, those
        // will be handled on the next call to ProcessMessages(). This was the simple thing to do.
        const auto queue = m_messageQueue;
        m_messageQueue.clear();

        // Go through the queue and dispatch the events.
        for (auto& message : queue)
        {
            // TODO: Make sure the message.pSender is still valid? Should that matter?

            // If we have listeners for that message type, send the message to the receivers.
            if (const auto result = m_messageListeners.find(message.id); result != m_messageListeners.end())
            {
                for (auto* pComponent : result->second)
                {
                    pComponent->ReceiveMessage(message);
                }
            }
        }
    }

    void MessageManager::AddListener(Component* pReceiver, const MessageId id)
    {
        m_messageListeners[id].emplace_back(pReceiver);
    }

    void MessageManager::RemoveListener(const Component* pReceiver, const MessageId id)
    {
        // Check to see if we have any receivers for that type.
        if (const auto result = m_messageListeners.find(id); result != m_messageListeners.end())
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

    void MessageManager::QueueMessage(Component* pSender, const MessageId id)
    {
        m_messageQueue.emplace_back(id, pSender);
    }
}