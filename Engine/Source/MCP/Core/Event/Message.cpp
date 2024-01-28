// Message.cpp

#include "Message.h"

namespace mcp
{
    Message::Message(const StringId id, Component* pSender)
        : pSender(pSender)
        , id(id)
    {
        //
    }
}