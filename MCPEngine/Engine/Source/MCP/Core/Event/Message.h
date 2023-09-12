#pragma once
// Message.h

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      A Message is a specific type of Event communication that takes place between components.
//
//      GOAL: Messages are a specific type of data. No inheritance. They need an Id, and a Component* at the very least.
//      I think collision messages might be a separate type of Message. That is how Unreal does it.
//
//-----------------------------------------------------------------------------------------------------------------------------

#include "utility/Hash.h"

namespace mcp
{
    class Component;
    using MessageIdType = uint32_t;

#define MCP_GET_MESSAGE_ID(MessageTypeName) HashString32(#MessageTypeName)

    static constexpr uint32_t GetMessageId(const char* messageTypeName) { return HashString32(messageTypeName); }

    struct Message
    {
        Component* const pSender;       // Which Component sent the message.
        const MessageIdType id;         // Id of the message type

        Message(const MessageIdType id, Component* pSender)
            : pSender(pSender)
            , id(id)
        {
            //
        }
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Do I need a MessengerId specific to this messenger? That way I can purge messages if need be.
    //
    ///		@brief : Messengers are simple classes that queue messages in the MessageManager of the scene.
    //-----------------------------------------------------------------------------------------------------------------------------
    class Messenger
    {
        Component* const m_pOwner;
        const MessageIdType m_id;

    public:
        Messenger(const MessageIdType id, Component* pOwner);
        ~Messenger();

        void SendMessage() const;
    };
}