#pragma once
#include <cstdint>

namespace mcp
{
    using ComponentTypeId = uint64_t;

    namespace ComponentInternal
    {
        ComponentTypeId GenerateComponentId(const char* pComponentName);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Generate a unique ComponentId based on the name of the ComponentType.
    ///		@param ComponentName : 
    //-----------------------------------------------------------------------------------------------------------------------------
#define MCP_DEFINE_COMPONENT_ID(ComponentName)                                                                      \
    static inline const ComponentTypeId kComponentTypeId = ComponentInternal::RegisterComponentType(#ComponentName);  \
    virtual ComponentTypeId GetTypeId() const override { return kComponentTypeId; }

}
