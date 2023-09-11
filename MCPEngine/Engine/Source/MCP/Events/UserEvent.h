#pragma once
// UserEvent.h
#include "Event.h"

//#include "utility/IdGenerator.h"

//#define MCP_DEFINE_USER_EVENT_INFO(eventName)                                   \
//    MCP_DEFINE_EVENT_TYPE(kUser)                                                \
//    inline static size_t s_eventId = GenerateId(#eventName);                    \
//    virtual size_t GetUserEventId() const override { return s_eventId; }

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : UserEvents are meant to be the base class for any custom events the user wants to create.
    ///         \n NOTE: When defining a User Event, use the macro MCP_DEFINE_USER_EVENT_INFO passing in the name of the
    ///         \n event name to properly set up the type to be used within the system.
    //-----------------------------------------------------------------------------------------------------------------------------
    struct UserEvent : public Event
    {
        UserEvent() = default;
        virtual ~UserEvent() override = default;
        //virtual size_t GetUserEventId() const = 0;

        MCP_DEFINE_EVENT_TYPE(kUser)
    };
}