#pragma once
// MessageManager.h

#include <unordered_map>
#include "Message.h"

namespace mcp
{
    class MessageManager 
    {
        using ReceiverArray = std::vector<Component*>;

        std::unordered_map<MessageIdType, ReceiverArray> m_messageReceivers;
        std::vector<Message> m_messageQueue;

    public:
        void ProcessMessages();
        void AddReceiver(Component* pReceiver, const MessageIdType id);
        void RemoveReceiver(const Component* pReceiver, const MessageIdType id);
        void QueueMessage(Component* pSender, const MessageIdType id);
    };
}