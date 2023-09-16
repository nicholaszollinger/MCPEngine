// ResourceManager.cpp

#include "ResourceManager.h"

namespace mcp
{
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
