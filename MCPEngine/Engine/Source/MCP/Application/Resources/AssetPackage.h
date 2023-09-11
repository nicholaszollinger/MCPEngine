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

    class AssetPackage // Zip file.
    {
        using Assets = std::unordered_map<std::string, RawData>;
        Assets m_packageAssets;

    public:
        AssetPackage() = default;
        ~AssetPackage();

        // TODO: Delete all copy and move constructors and assignment operators.

        bool LoadPackage(const char* pZipFileName);
        RawData* GetRawData(const char* pFileName);

    private:
        void FreePackageData();
    };
}