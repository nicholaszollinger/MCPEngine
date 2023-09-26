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

#include "Utility/String/StringId.h"

namespace mcp
{
    class Component;
    using MessageId = StringId;

    struct Message
    {
        Component* const pSender;   // Which Component sent the message.
        const MessageId id;         // Id of the message type

        Message(const StringId id, Component* pSender)
            : pSender(pSender)
            , id(id)
        {
            //
        }
    };
}