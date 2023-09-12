#pragma once
// ComponentFactory.h

#include <cassert>
#include <functional>
#include <unordered_map>
#include "MCP/Debug/Log.h"
#include "utility/Hash.h"

namespace mcp
{
    class Object;

    using ComponentTypeId = uint64_t;

    class ComponentFactory
    {
        using FactoryFunction = std::function<bool(const void*, Object*)>;
        static inline std::unordered_map<ComponentTypeId, FactoryFunction> s_factoryFunctions {};

    public:
        static void RegisterEngineComponentTypes();

        static bool AddToObjectFromData(const char* pComponentName, const void* pData, Object* pOwner)
        {
            const ComponentTypeId id = HashString32(pComponentName);

            const auto result = s_factoryFunctions.find(id);
            if (result == s_factoryFunctions.end())
            {
                LogError("Failed to add '%' to object! No matching ComponentId was found!", pComponentName);
                return false;
            }

            return result->second(pData, pOwner);
        }

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Registers the component type with the ComponentFactory. A unique Id is generated based on the Component's
        ///             name, and the static member function 'ComponentType::AddFromData()' is registered as the callback for
        ///             adding this component to an Object from file data.
        ///		@returns : A unique id for this ComponentType.
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename ComponentType>
        static ComponentTypeId RegisterComponentType(const char* pComponentName)
        {
            const ComponentTypeId id = HashString32(pComponentName);

            // Check to see if we already have that id in our map:
            if (const auto result = s_factoryFunctions.find(id); result != s_factoryFunctions.end())
            {
                //LogError("Failed to register Component! Id from the name '%' was already registered! This could mean that you have two components with the same name!", pComponentName);
                assert(false && "Failed to register Component! This could mean that you have two components with the same name!"); // I don't know how to handle this error.
            }

            s_factoryFunctions.emplace(id, [](const void* pData, class Object* pOwner) { return ComponentType::AddFromData(pData, pOwner); });

            return id;
        }
    };
}