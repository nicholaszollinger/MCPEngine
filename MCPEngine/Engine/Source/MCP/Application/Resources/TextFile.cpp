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
    template<>
    TextFileAssetType* ResourceContainer<TextFileAssetType>::LoadFromDiskImpl(const char* pFilePath)
    {
        std::fstream stream;
        stream.open(pFilePath);
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
    template<>
    TextFileAssetType* ResourceContainer<TextFileAssetType>::LoadFromRawDataImpl(char* pData, const int dataSize)
    {
        auto* pString = BLEACH_NEW(std::string);
        pString->reserve(dataSize);

        *pString = pData;

        return pString;
    }

    template<>
    void ResourceContainer<TextFileAssetType>::FreeResourceImpl(TextFileAssetType* pAsset)
    {
        BLEACH_DELETE(pAsset);
    }

    TextFile::~TextFile()
    {
        if (m_pResource)
        {
            Free();
        }
    }

    void TextFile::Load(const char* pFilePath, const char* pPackageName, const bool isPersistent)
    {
        if (m_pResource)
        {
            Free();
        }

        m_loadData.pFilePath = pFilePath;
        m_loadData.isPersistent = isPersistent;
        m_loadData.pPackageName = pPackageName;

        m_pResource = ResourceManager::Get()->Load<TextFileAssetType>(m_loadData);
    }

    void TextFile::Free()
    {
        ResourceManager::Get()->FreeResource<TextFileAssetType>(m_loadData.pFilePath);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Write the contents of the text file to the console.
    //-----------------------------------------------------------------------------------------------------------------------------
    void TextFile::Dump() const
    {
        std::cout << *(static_cast<std::string*>(m_pResource)) << "\n";
    }
}