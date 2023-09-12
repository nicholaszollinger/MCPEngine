#pragma once
// ColliderFactory.h

#include <cassert>
#include <functional>
#include "MCP/Debug/Log.h"
#include "utility/Hash.h"

namespace mcp
{
    class Collider;
    class ColliderComponent;

    using ColliderTypeId = uint32_t;
    static constexpr ColliderTypeId kInvalidColliderTypeId = std::numeric_limits<ColliderTypeId>::max();

    class ColliderFactory
    {
        using FactoryFunction = std::function<bool(const void*, mcp::ColliderComponent*)>;
        static inline std::unordered_map<ColliderTypeId, FactoryFunction> s_factoryFunctions = {};

    public:

        static bool AddNewFromData(const char* pColliderTypeName, const void* pData, mcp::ColliderComponent* pComponent)
        {
            const ColliderTypeId id = HashString32(pColliderTypeName);

            const auto result = s_factoryFunctions.find(id);
            if (result == s_factoryFunctions.end())
            {
                LogError("Failed create '%'! No matching ColliderTypeId was found!", pColliderTypeName);
                return false;
            }

            // Return the result of the create function for that type.
            return result->second(pData, pComponent);
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
            //const ColliderTypeId id = HashString(pColliderTypeName);
            const ColliderTypeId id = HashString32(pColliderTypeName);

            // Check to see if we already have the id in our map:
            if (const auto result = s_factoryFunctions.find(id); result != s_factoryFunctions.end())
            {
                //LogError("Failed to register ColliderType! Id from the name '%' was already registered! This could mean that you have two colliders with the same name!", pColliderTypeName);
                assert(false && "Failed to register Collider! This could mean that you have two colliders with the same name!"); 
            }

            s_factoryFunctions.emplace(id, [](const void* pData, mcp::ColliderComponent* pComponent ){ return ColliderType::AddFromData(pData, pComponent); });

            return id;
        }
        
        static void RegisterEngineColliderTypes();
    };
}