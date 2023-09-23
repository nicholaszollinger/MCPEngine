#pragma once
// ColliderFactory.h

#include <cassert>
#include <functional>
#include "MCP/Debug/Log.h"
#include "MCP/Core/Resource/Parser.h"
#include "Utility/Generic/Hash.h"

namespace mcp
{
    class Collider;
    class ColliderComponent;

    using ColliderTypeId = uint32_t;
    static constexpr ColliderTypeId kInvalidColliderTypeId = std::numeric_limits<ColliderTypeId>::max();

    class ColliderFactory
    {
        using FactoryFunction = std::function<bool(const XMLElement, mcp::ColliderComponent*)>;
        using FactoryFuncContainer = std::unordered_map<ColliderTypeId, FactoryFunction>;

    public:
        static bool AddNewFromData(const char* pColliderTypeName, const XMLElement colliderData, mcp::ColliderComponent* pComponent)
        {
            const ColliderTypeId id = HashString32(pColliderTypeName);

            FactoryFuncContainer& factoryFunctions = GetFactoryContainer();

            const auto result = factoryFunctions.find(id);
            if (result == factoryFunctions.end())
            {
                MCP_ERROR("Collision", "Failed create '", pColliderTypeName, "'! No matching ColliderTypeId was found!");
                return false;
            }

            // Return the result of the create function for that type.
            return result->second(colliderData, pComponent);
        }
        
        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //		
        ///		@brief : Registers the ColliderType with the ColliderFactory. We are getting a reference to
        ///             the Collider's static 'AddFromData' function so that we can determine what colliders we want
        ///             to create based off a c-string version of the Type.
        ///		@tparam ColliderType : Type of collider we are registering.
        ///     @param pColliderTypeName : String version of the ColliderType, used just for logging.
        ///     @returns : A unique id for this ColliderType.
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename ColliderType>
        static ColliderTypeId RegisterColliderType(const char* pColliderTypeName)
        {
            const ColliderTypeId id = HashString32(pColliderTypeName);

            FactoryFuncContainer& factoryFunctions = GetFactoryContainer();

            // Check to see if we already have the id in our map:
            if (const auto result = factoryFunctions.find(id); result != factoryFunctions.end())
            {
                //LogError("Failed to register ColliderType! Id from the name '%' was already registered! This could mean that you have two colliders with the same name!", pColliderTypeName);
                assert(false && "Failed to register Collider! This could mean that you have two colliders with the same name!"); 
            }

            factoryFunctions.emplace(id, [](const XMLElement colliderData, mcp::ColliderComponent* pComponent ){ return ColliderType::AddFromData(colliderData, pComponent); });

            return id;
        }

    private:
        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //      This is to force the container to exist. I had an issue where we were trying to register a collider BEFORE our
        //      static factory function container existed. This forces the container to exist when we ask for it!
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