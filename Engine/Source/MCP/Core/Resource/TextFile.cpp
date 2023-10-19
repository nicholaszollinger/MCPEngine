// TextFile.cpp

#include "TextFile.h"

#include <BleachNew.h>
#include <filesystem>
#include <fstream>
#include <string_view>
#include "ResourceManager.h"
#include "Zip.h"

#include <iostream>

namespace mcp
{
    struct TextFileAsset
    {
        const char* pFilePath = nullptr;
        std::fstream stream;
    };

    using TextFileAssetType = std::string;
    
    template<>
    TextFileAssetType* ResourceContainer<TextFileAssetType, DiskResourceRequest>::LoadFromDiskImpl(const DiskResourceRequest& request)
    {
        std::fstream stream;
        stream.open(request.path.GetCStr());
        if (!stream.is_open())
        {
            // Report an error.
            return nullptr;
        }

        auto* pString = BLEACH_NEW(std::string);

        while(std::getline(stream, *pString))
        {
            
        }

        return pString;
    }
    
    template<>
    TextFileAssetType* ResourceContainer<TextFileAssetType, DiskResourceRequest>::LoadFromRawDataImpl(char* pData, const int dataSize, [[maybe_unused]] const DiskResourceRequest& request)
    {
        auto* pString = BLEACH_NEW(std::string);
        pString->reserve(dataSize);

        *pString = pData;

        return pString;
    }

    template<>
    void ResourceContainer<TextFileAssetType, DiskResourceRequest>::FreeResourceImpl(TextFileAssetType* pAsset)
    {
        BLEACH_DELETE(pAsset);
    }

    void* TextFile::LoadResourceType()
    {
        return ResourceManager::Get()->LoadFromDisk<TextFileAssetType>(m_request);
    }


    void TextFile::Free()
    {
        ResourceManager::Get()->FreeResource<TextFileAssetType>(m_request);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Write the contents of the text file to the console.
    //-----------------------------------------------------------------------------------------------------------------------------
    void TextFile::Dump() const
    {
        MCP_LOG("TextFile", *(static_cast<std::string*>(m_pResource)));
    }
}