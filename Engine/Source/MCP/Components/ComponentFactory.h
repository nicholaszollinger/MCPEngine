#pragma once
// ComponentFactory.h

#include <cassert>
#include <functional>
#include <unordered_map>
#include "MCP/Core/Resource/Parsers/XMLParser.h"
#include "MCP/Debug/Log.h"
#include "Utility/Generic/Hash.h"

namespace mcp
{
    class Object;

    using ComponentTypeId = uint64_t;

    class ComponentFactory
    {
        using FactoryFunction = std::function<bool(XMLElement, Object*)>;
        using FactoryFuncContainer = std::unordered_map<ComponentTypeId, FactoryFunction>;

    public:
        static bool AddToObjectFromData(const char* pComponentName, const XMLElement component, Object* pOwner)
        {
            const ComponentTypeId id = HashString32(pComponentName);

            FactoryFuncContainer& factoryFunctions = GetFactoryContainer();

            const auto result = factoryFunctions.find(id);
            if (result == factoryFunctions.end())
            {
                MCP_ERROR("ComponentFactory", "Failed to add '", pComponentName, "' to object! No matching ComponentId was found!");
                return false;
            }

            return result->second(component, pOwner);
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

            FactoryFuncContainer& factoryFunctions = GetFactoryContainer();

            // Check to see if we already have that id in our map:
            if (const auto result = factoryFunctions.find(id); result != factoryFunctions.end())
            {
                //LogError("Failed to register Component! Id from the name '%' was already registered! This could mean that you have two components with the same name!", pComponentName);
                assert(false && "Failed to register Component! This could mean that you have two components with the same name!"); // I don't know how to handle this error.
            }

            factoryFunctions.emplace(id, [](const XMLElement component, class Object* pOwner) { return ComponentType::AddFromData(component, pOwner); });

            return id;
        }

    private:
        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //      This is to force the container to exist. I had an issue where we were trying to register a component BEFORE our
        //      static factory function container existed. This forces the container to exist when we ask for it.
        //		
        ///		@brief : Get a reference to the static container of factory functions.
        //-----------------------------------------------------------------------------------------------------------------------------
        static FactoryFuncContainer& GetFactoryContainer()
        {
            static FactoryFuncContainer factoryFunctions {};
            return factoryFunctions;
        }
    };
}