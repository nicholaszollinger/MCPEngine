// ComponentId.cpp

#include "ComponentId.h"

#include <cassert>
#include <unordered_map>
#include "MCP/Debug/Log.h"
#include "utility/IdGenerator.h"

namespace mcp::ComponentInternal
{
#ifdef _DEBUG
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Debug version checks to make sure that we don't have any of the same names already created.
    //      This will throw an assertion to halt the program.
    //
    ///		@brief : Generates a ComponentTypeId based on the name of the component.
    ///         \n NOTE: This is for internal use only! Use Macro: MCP_DEFINE_COMPONENT_ID in the body of the
    ///             Component class to properly set up the Component's unique Id.
    //-----------------------------------------------------------------------------------------------------------------------------
    ComponentTypeId GenerateComponentId(const char* pComponentName)
    {
        static std::unordered_map<const char*, ComponentTypeId> sIdMap;

        // Debug check to see if we have a unique id or not.
        if (const auto result = sIdMap.find(pComponentName); result != sIdMap.end())
        {
            LogError("Component with name: '%' , is already defined!");
            assert(false); // Assert false to quit in debug.
        }
       
        return GenerateId(pComponentName);
    }

#else
    //-----------------------------------------------------------------------------------------------------------------------------
    //
    ///		@brief : Generates a ComponentTypeId based on the name of the component.
    ///         \n NOTE: This is for internal use only! Use Macro: MCP_DEFINE_COMPONENT_ID() in the body of the
    ///             Component class to properly set up the Component's unique Id.
    //-----------------------------------------------------------------------------------------------------------------------------
    ComponentTypeId GenerateComponentId(const char* pComponentName)
    {
        return GenerateId(pComponentName);
    }
#endif
}
