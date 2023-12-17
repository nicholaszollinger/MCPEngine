// ComponentId.cpp

#include "ComponentId.h"

#include <cassert>
#include <unordered_map>

#include "MCP/Debug/Assert.h"
#include "MCP/Debug/Log.h"
#include "Utility/Generic/Hash.h"

namespace mcp::ComponentInternal
{
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
#if _DEBUG
        static std::unordered_map<const char*, ComponentTypeId> sIdMap;

        // Debug check to see if we have a unique id or not.
        if (const auto result = sIdMap.find(pComponentName); result != sIdMap.end())
        {
            MCP_ERROR("ComponentId", "Component with name: ", pComponentName, ", is already defined!");
            MCP_CHECK(false); // Assert false to quit in debug.
        }
#endif
        return HashString64(pComponentName);
    }
}
