#pragma once
// MessageManager.h

#include <unordered_map>
#include "Message.h"

namespace mcp
{
    class MessageManager 
    {
        using Listeners = std::vector<Component*>;

        std::unordered_map<MessageId, Listeners, StringIdHasher> m_messageListeners;
        std::vector<Message> m_messageQueue;

    public:
        void ProcessMessages();
        void AddListener(Component* pReceiver, const MessageId id);
        void RemoveListener(const Component* pReceiver, const MessageId id);
        void QueueMessage(Component* pSender, const MessageId id);
    };
}
