#pragma once
// EventType.h
// I want to set up my own event structure, with simple flags to denote types.

#include <functional>
#include <unordered_map>
#include "../Application/Core/Macros.h"
#include "../Application/Debug/Log.h"
#include "utility/ConceptTypes.h"

GENERATE_DEPENDENT_TYPE_CHECK(CanCallEvent, EventType, FuncType, std::declval<FuncType>()(std::declval<const EventType&>()));

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Helper macro that creates a lambda to encapsulate the member function and adds it to the global EventDispatcher.
///		@param eventType : Name of the EventType.
///		@param functionName : Name of the function that you want to bind to the event.
//-----------------------------------------------------------------------------------------------------------------------------
#define MCP_ADD_MEMBER_FUNC_EVENT_LISTENER(eventType, functionName) \
    mcp::EventDispatcher::AddListener<eventType>(this, [this](const eventType& e) { this->functionName(e); })

namespace mcp
{
    enum class EventType
    {
        kInvalid =  0,
        KQuit       = MCP_BIT(1),
        kWindow     = MCP_BIT(2),
        kKey        = MCP_BIT(3),
        kMouse      = MCP_BIT(4),
        kUser       = MCP_BIT(5),
    };

    #define MCP_DEFINE_EVENT_TYPE(type)                                             \
        static EventType GetStaticType() { return EventType::type; }                \
        virtual EventType GetEventType() const override { return GetStaticType(); }


    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Events are required to have a single virtual function, but I still view the construct as a
    ///     container of data. Getting it's underlying type is generally the only thing we are concerned about.
    //
    ///		@brief : EventType base struct. Structs that inherit from this should use the macro 'MCP_DEFINE_EVENT_TYPE' to have the
    ///             structure set up correctly. Simply pass one of the EventType from the enumeration and the EventType is ready to go.
    ///             Events should just be containers for data. Nothing more.
    //-----------------------------------------------------------------------------------------------------------------------------
    struct Event
    {
        Event() = default;
        Event(const Event& right) = default;
        Event(Event&& right) = default;
        Event& operator=(const Event& right) = default;
        Event& operator=(Event&& right) = default;
        virtual ~Event() = default;

        [[nodiscard]] virtual EventType GetEventType() const = 0;

        // TODO: virtual Debug ToString function.
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : This is our global way for managing events. Each static function is templated so that this single class
    ///         can handle any kind of EventType that we want to have global in the engine. This can be useful for events like
    ///         Input.
    //-----------------------------------------------------------------------------------------------------------------------------
    class EventDispatcher
    {
        template<typename EventType>
        using Listeners = std::unordered_map<void*, std::function<void(const EventType&)>>;

    public:
        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Invokes the event immediately.
        ///		@tparam EventType : Type of EventType.
        ///		@param e : EventType parameter.
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename EventType>
        static void InvokeNow(const EventType& e)
        {
            Invoke(e);
        }

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Bind a function to be called on a certain EventType type.
        ///		@tparam EventType : Type of event you want to hook in to.
        ///		@tparam Func : Function you want called for that event. Must have the signature "
        ///		@param pOwner : Owner of the function. Generally the 'this' pointer.
        ///		@param func : Function you want to bind to this type of event.
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename EventType, typename Func>
        static void AddListener(void* pOwner, const Func& func)
        {
            static_assert(CanCallEvent<EventType, Func>::value, "Function must have the signature: Func(const EventType& event);");

            GetListeners<EventType>().emplace(pOwner, func);
        }

        template<typename EventType>
        static void RemoveListener(void* pOwner)
        {
            auto& listeners = GetListeners<EventType>();
            if (auto result = listeners.find(pOwner); result != listeners.end())
            {
                listeners.erase(pOwner);
            }

            else
            {
                // TODO: Make a better warning message?
                LogWarning("Attempted to remove listener that doesn't exist.");
            }
        }

    private:
        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Get a reference to the Listeners of an event type. 
        ///		@tparam EventType : Type of EventType.
        ///		@returns : Static unordered_map reference of Listeners.
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename EventType>
        static auto& GetListeners()
        {
            // Interestingly, having the data statically live the function here allows us
            // to templatize it without having to make the entire class a template.
            static Listeners<EventType> listeners;
            return listeners;
        }

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Invoke the EventType, calling the bound function of each listener.
        ///		@tparam EventType : Type of EventType.
        ///		@param e : EventType parameter.
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename EventType>
        static void Invoke(const EventType& e)
        {
            auto& listeners = GetListeners<EventType>();
            // This isn't necessarily that quick to plow through, since it is an unordered map.
            // We'll revisit this if necessary.
            for (auto& listener : listeners)
            {
                listener.second(e);
            }
        }
    };

}