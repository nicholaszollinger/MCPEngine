// AssetPackage.cpp

#include "AssetPackage.h"

#include <BleachNew.h>
#include <cassert>
#include <fstream>
#define ZLIB_WINAPI
#include <zlib.h>
#include "MCP/Application/Debug/Log.h"
#include "MCP/Application/Resources/Zip.h"

namespace mcp
{
    AssetPackage::~AssetPackage()
    {
        // Free the stored data in this package from memory.
        FreePackageData();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : LoadPackage a package's assets into memory. This will put all of the uncompressed data into memory!
    ///		@param pZipFileName : name of the zip file.
    ///		@returns : True on success, false on failure.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool AssetPackage::LoadPackage(const char* pZipFileName)
    {
        std::fstream stream;
        stream.open(pZipFileName, std::ios::in | std::ios::binary);
        if (!stream.is_open())
        {
            LogError("Failed to load AssetPackage! Failed to open file: '%'", pZipFileName);
            return false;
        }

        ZipHeader zipHeader; // This will zero everything out.

        // Go to the end of the zip file, then move back by the size of the zip header.
        constexpr std::streamoff kZipHeaderOffset = -static_cast<std::streamoff>(sizeof(zipHeader));
        stream.seekg(kZipHeaderOffset, std::ios::end);

        // Populate the zipHeader information by reading directly into the zipHeader
        stream.read(reinterpret_cast<char*>(&zipHeader), sizeof(zipHeader));

        // Make sure that we have the correct zip signature.
        if (zipHeader.signature != kZipSignature)
        {
            LogError("Failed to load AssetPackage! Package file was not a .zip!");
            stream.close();
            return false;
        }

        std::streamoff dirOffset = zipHeader.dirOffset;

        // Start reading from the spot of the file headers.
        stream.seekg(dirOffset, std::ios::beg);

        // LoadPackage in the directory data in a buffer.
        char* pDirData = BLEACH_NEW_ARRAY(char, zipHeader.dirSize);
        memset(pDirData, 0, zipHeader.dirSize);
        stream.read(pDirData, zipHeader.dirSize);

        // Test the first file:
        FileHeader* pFileHeader = reinterpret_cast<FileHeader*>(pDirData);
        if (pFileHeader->signature != kFileSignature)
        {
            LogError("Failed to load AssetPackage! Failed to get FileHeader!");
            stream.close();
            BLEACH_DELETE_ARRAY(pDirData);
            return false;
        }

        // For each file, we are going to get the name, decompress the data and store both in our assets map.
        for (int i = 0; i < zipHeader.numberOfFiles; ++i)
        {
            // Each file header is a different size, because of the name of the file at the end.
            // We need to move over 1 FileHeader size over, then we need to move over the 
            // nameLength times the byte size.
            char* pNamePtr = reinterpret_cast<char*>(pFileHeader + 1);

            // Construct the Name.
            char* pName = BLEACH_NEW_ARRAY(char, pFileHeader->nameLength + 1);
            memcpy(pName, pNamePtr, pFileHeader->nameLength);
            pName[pFileHeader->nameLength] = '\0';

            // Getting the data header
            DataHeader dataHeader;
            stream.seekg(pFileHeader->dataOffset, std::ios::beg);
            stream.read(reinterpret_cast<char*>(&dataHeader), sizeof(dataHeader));

            // If we failed to get a data header signature, then something went horribly wrong and we need to quit.
            if (dataHeader.signature != kDataSignature)
            {
                LogError("Failed to load AssetPackage! Failed to get DataHeader of file: '%'", pName);
                FreePackageData();
                BLEACH_DELETE_ARRAY(pName);
                stream.close();
                return false;
            }

            // Find the spot where the compressed data lives in the zip file and read it into a buffer.
            stream.seekg(dataHeader.nameLength, std::ios::cur);
            char* pData = BLEACH_NEW_ARRAY(char, dataHeader.compressedSize);
            stream.read(pData, dataHeader.compressedSize);

            // If we are looking at compressed data, decompress it.
            if (dataHeader.compression == Z_DEFLATED)
            {
                // Create a buffer equal to the uncompressed size.
                char* pUncompressedData = BLEACH_NEW_ARRAY(char, dataHeader.uncompressedSize);

                // Set up a zStream
                z_stream zStream;
                zStream.next_in = reinterpret_cast<Bytef*>(pData);
                zStream.avail_in = dataHeader.compressedSize;
                zStream.next_out = reinterpret_cast<Bytef*>(pUncompressedData);
                zStream.avail_out = dataHeader.uncompressedSize;
                zStream.zalloc = static_cast<alloc_func>(nullptr); // Use the default allocator.
                zStream.zfree = static_cast<free_func>(nullptr);   // Use the default deallocator.

                // Perform the inflation.
                int zStatus = inflateInit2(&zStream, -MAX_WBITS);
                if (zStatus == Z_OK)
                {
                    zStatus = inflate(&zStream, Z_FINISH);
                    inflateEnd(&zStream);

                    if (zStatus == Z_STREAM_END)
                    {
                        zStatus = Z_OK;
                    }

                    // You have to end the inflation again. Don't know why.
                    inflateEnd(&zStream);
                }

                // Free the compressed data from memory.
                BLEACH_DELETE_ARRAY(pData);

                // Set the Uncompressed data as our pData.
                pData = pUncompressedData;
            }

            // Add the name and pData to our map:
            m_packageAssets.emplace(std::make_pair(pName, RawData{ pData , static_cast<int>(dataHeader.uncompressedSize)}));

            // TODO: we don't need this extra allocation. Make the string above.
            // Delete the allocated array.
            BLEACH_DELETE_ARRAY(pName);

            // Update the position of our file header.
            pFileHeader = reinterpret_cast<FileHeader*>(pNamePtr + pFileHeader->nameLength);
        }

        stream.close();
        BLEACH_DELETE_ARRAY(pDirData);

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Returns the raw data in memory that matches the file name. If the filename does not exist, then
    ///            it returns nullptr.
    //-----------------------------------------------------------------------------------------------------------------------------
    RawData* AssetPackage::GetRawData(const char* pFileName)
    {
        const auto result = m_packageAssets.find(pFileName);
        if (result == m_packageAssets.end())
        {
            LogError("Failed to find asset in package! Filename: '%'", pFileName);
            return nullptr; // Returns an invalid piece of data.
        }

        return &result->second;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Frees the data and the dynamically allocated names from memory.
    //-----------------------------------------------------------------------------------------------------------------------------
    void AssetPackage::FreePackageData()
    {
        for (auto [pName, data] : m_packageAssets)
        {
            // Delete the uncompressed data from memory.
            BLEACH_DELETE_ARRAY(data.pData);
        }
    }
}