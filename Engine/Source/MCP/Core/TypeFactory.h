#pragma once
// TypeFactory.h

#include <functional>
#include "MCP/Core/Resource/Parsers/XMLParser.h"

namespace mcp
{
    using TypeId = uint64_t;

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      I have this pattern in several places in my code base: Components, Widgets, Colliders. Seemed reasonable to standardize it.
    //		
    ///		@brief : Templated factory to register types used in the Engine and in the Game.
    ///		@tparam BaseType : The base type that this factory produces. Example: 'Component' or 'Widget'
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename BaseType>
    class TypeFactory
    {
        using CreateFromData = std::function<BaseType*(const XMLElement)>;
        using CreateDefault = std::function<BaseType*()>;

        struct TypeCreationData
        {
            CreateFromData createFromData;
            CreateDefault createDefault = nullptr;
        };

        using FactoryFunctionContainer = std::unordered_map<TypeId, TypeCreationData>;

    public:
        static BaseType* CreateTypeFromData(const char* pTypename, const XMLElement element)
        {
            const TypeId id = HashString64(pTypename);
            auto& factoryFunctions = GetFactoryContainer();

            const auto result = factoryFunctions.find(id);
            if (result == factoryFunctions.end())
            {
                MCP_ERROR("TypeFactory", "Failed to create ", pTypename, " from data! No matching type id found!");
                return nullptr;
            }

            return result->second.createFromData(element);
        }

        static BaseType* CreateDefaultType(const char* pTypename)
        {
            const TypeId id = HashString64(pTypename);
            auto& factoryFunctions = GetFactoryContainer();

            const auto result = factoryFunctions.find(id);
            if (result == factoryFunctions.end())
            {
                MCP_ERROR("TypeFactory", "Failed to create default ", pTypename, "! No matching type id found!");
                return nullptr;
            }

            // If there was no default constructor available, then return nullptr
            if (!result->second.createDefault)
            {
                MCP_ERROR("TypeFactory", "Failed to create default ", pTypename, "! Type is not default constructible!");
                return nullptr;
            }

            // Otherwise, return the a new default constructed type
            return result->second.createDefault();
        }

        template<typename DerivedType>
        static TypeId RegisterDerivedType(const char* pDerivedTypename)
        {
            const TypeId id = HashString64(pDerivedTypename);
            auto& factoryFunctions = GetFactoryContainer();

            // Check to see if we already have that id in our map:
            const auto result = factoryFunctions.find(id);

            // TODO: I should probably throw a critical failure here:
            MCP_CHECK_MSG(result == factoryFunctions.end(), "Failed to register Type: '", pDerivedTypename, "'! We already had a Type by that id registered. This could mean that we have two of the same name!");

            // Register the creation functions:
            TypeCreationData functions;
            functions.createFromData = [](const XMLElement element) -> DerivedType* { return DerivedType::AddFromData(element); };

            // If the type is default constructable, add a functor to be able to create the derived type. 
            if constexpr (std::is_default_constructible_v<DerivedType>)
            {
                functions.createDefault = []() -> DerivedType* { return BLEACH_NEW(DerivedType); };
            }

            factoryFunctions.emplace(id, functions);

            return id;
        }

    private:
        static FactoryFunctionContainer& GetFactoryContainer()
        {
            static FactoryFunctionContainer container;
            return container;
        }
    };

#define MCP_REGISTER_TYPE(factory, typename)                                                           \
private:                                                                                               \
    static inline const mcp::TypeId kTypeId = mcp::factory::RegisterDerivedType<typename>(#typename);  \
                                                                                                       \
public:                                                                                                \
    static mcp::TypeId GetStaticTypeId() { return kTypeId; }                                           \
    virtual mcp::TypeId GetTypeId() const override { return kTypeId; }                                 \
private:  

}