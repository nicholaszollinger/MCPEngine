#pragma once
// PackageManager.h

#include <unordered_map>
#include "AssetPackage.h"

namespace mcp
{
    class PackageManager
    {
        // Create a handle to delete packages maybe.
        using PackageMap = std::unordered_map<const char*, AssetPackage*>;

        static inline PackageManager* s_pInstance = nullptr;
        PackageMap m_packages;

    public:
        // TODO: Delete all copy and move constructors and assignment operators.

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