// ResourceManager.cpp

#include "ResourceManager.h"

#include <BleachNew.h>


namespace mcp
{
    // This is the max allowable filename size (including the path) on windows.
    //constexpr size_t kMaxNameSize = 260; 

    /*void ResourceManager::Create()
    {
        if (!s_pInstance)
        {
            s_pInstance = BLEACH_NEW(ResourceManager());
        }
    }

    void ResourceManager::Destroy()
    {
        BLEACH_DELETE(s_pInstance);
        s_pInstance = nullptr;

        PackageManager::Destroy();
    }*/

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
