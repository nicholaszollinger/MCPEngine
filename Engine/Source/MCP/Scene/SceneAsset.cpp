// SceneAsset.cpp

#include "SceneAsset.h"

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the result of the attribute "isDebugOnly" for this element. If no attribute exists, this returns false.
    ///		@param element : Element we are checking.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool AssetIsDebugOnly(const XMLElement element)
    {
        return element.GetAttributeValue<bool>("isDebugOnly", false);
    }

}