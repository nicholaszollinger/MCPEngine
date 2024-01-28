#pragma once
// PackageManager.h

#include <unordered_map>
#include "AssetPackage.h"
#include "Utility/String/StringId.h"

namespace mcp
{
    class PackageManager
    {
        // TODO: Create a handle for packages. StringId.
        using PackageMap = std::unordered_map<StringId, AssetPackage*, StringIdHasher>;

        static inline PackageManager* s_pInstance = nullptr;
        PackageMap m_packages;

    public:
        PackageManager(const PackageManager&) = delete;
        PackageManager(PackageManager&&) = delete;
        PackageManager& operator=(const PackageManager&) = delete;
        PackageManager& operator=(PackageManager&&) = delete;

    private:
        PackageManager() = default;
        ~PackageManager();
    public:
        static void Create();
        static PackageManager* Get() { return s_pInstance; }
        static void Destroy();

        bool LoadPackage(const char* pZipFileName);
        void UnloadPackage(const char* pZipFileName);
        RawData* GetRawData(const char* pPackagePath, const char* pFileName);
    };
}