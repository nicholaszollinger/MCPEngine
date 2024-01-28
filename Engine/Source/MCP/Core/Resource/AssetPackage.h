#pragma once
// AssetPackage.h

#include <string>  // I don't want this include here, may need to change the structure.
#include <unordered_map>

namespace mcp
{
    struct DataHeader;

    struct RawData
    {
        char* pData = nullptr;
        int size = 0;
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : On disk, an AssetPackage is a .zip file. This class unzips the raw data and creates mappings to that data for
    ///         to each resource.
    //-----------------------------------------------------------------------------------------------------------------------------
    class AssetPackage
    {
        using Assets = std::unordered_map<std::string, RawData>;
        Assets m_packageAssets;

    public:
        AssetPackage() = default;
        ~AssetPackage();

        AssetPackage(const AssetPackage&) = delete;
        AssetPackage(AssetPackage&&) = delete;
        AssetPackage& operator=(const AssetPackage&) = delete;
        AssetPackage& operator=(AssetPackage&&) = delete;

        bool LoadPackage(const char* pZipFileName);
        RawData* GetRawData(const char* pFileName);

    private:
        void FreePackageData();
    };
}