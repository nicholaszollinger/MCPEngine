// PackageManager.cpp

#include "PackageManager.h"

#include <BleachNew.h>
#include "AssetPackage.h"
#include "MCP/Debug/Log.h"

namespace mcp
{
    void PackageManager::Create()
    {
        if (!s_pInstance)
        {
            s_pInstance = BLEACH_NEW(PackageManager);
        }
    }

    void PackageManager::Destroy()
    {
        BLEACH_DELETE(s_pInstance);
        s_pInstance = nullptr;
    }

    PackageManager::~PackageManager()
    {
        for (auto& [pName, pPackage] : m_packages)
        {
            BLEACH_DELETE(pPackage);
        }
    }

    bool PackageManager::LoadPackage(const char* pZipFileName)
    {
        // If we already have that package loaded into memory, don't load it.
        if (const auto result = m_packages.find(pZipFileName); result != m_packages.end())
        {
            MCP_WARN("PackageManager", "Tried to reload a package with name: ", pZipFileName);
            return true;
        }

        auto* pPackage = BLEACH_NEW(AssetPackage);

        if (!pPackage->LoadPackage(pZipFileName))
        {
            MCP_WARN("PackageManager", "Failed to load package: ", pZipFileName);
            return false;
        }
        
        m_packages.emplace(pZipFileName, pPackage);
        return true;
    }

    void PackageManager::UnloadPackage(const char* pZipFileName)
    {
        const auto result = m_packages.find(pZipFileName);
        if (result == m_packages.end())
        {
            MCP_WARN("PackageManager", "Tried to unload a package not loaded with name: ", pZipFileName);
            return;
        }

        BLEACH_DELETE(result->second);
        m_packages.erase(result);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Get the raw data from the package specified. If the package isn't loaded, we will load it here. Make sure
    ///             to pre-load!
    ///		@param pPackagePath : Name of the package we are getting the data from.
    ///		@param pFileName : Name of the file we are trying to access.
    ///		@returns : Ptr to the raw data on success, otherwise nullptr.
    //-----------------------------------------------------------------------------------------------------------------------------
    RawData* PackageManager::GetRawData(const char* pPackagePath, const char* pFileName)
    {
        auto result = m_packages.find(pPackagePath);
        if (result == m_packages.end())
        {
            MCP_WARN("PackageManager", "Trying to get data from unloaded package. Loading now...\nPackage name: ", pPackagePath);

            if (!LoadPackage(pPackagePath))
            {
                // Load package will take care of the error msg.
                return nullptr;
            }

            result = m_packages.find(pPackagePath);
        }

        AssetPackage* pPackage = result->second;

        auto* pRawData = pPackage->GetRawData(pFileName);
        if (!pRawData)
        {
            MCP_ERROR("AssetPackage", "Failed to find asset with name: ", pFileName, ", in package named: ", pPackagePath);
            return nullptr;
        }

        return pRawData;
    }

}
