// ResourceManager.cpp

#include "ResourceManager.h"
#include "MCP/Core/Application/Application.h"

namespace mcp
{
    MCP_DEFINE_STATIC_SYSTEM_GETTER(ResourceManager)

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Creates the PackageManager instance.
    ///		@returns : True on success, false if the directory could not be opened.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool ResourceManager::Init()
    {
        PackageManager::Create();
        return true;
    }

    void ResourceManager::Close()
    {
        PackageManager::Destroy();
    }

}
