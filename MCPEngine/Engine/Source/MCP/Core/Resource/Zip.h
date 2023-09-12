#pragma once
// Zip.h
#include <cstdint>

// In the future, this will be able to compress my assets into a nice zip file.
// Right now, I simply need to load a file, expand it, then when closing, I need to compress
// it, and then save it.

namespace mcp
{
    static constexpr uint32_t kZipSignature = 0x06054b50;
    static constexpr uint32_t kFileSignature = 0x02014b50;
    static constexpr uint32_t kDataSignature = 0x04034b50;

#pragma pack(1) // <- This tells VS that we don't want any optimizations or any extra data for this struct.
    struct ZipHeader
    {
        // The order of the data MATTERS and the EXACT SIZE.
        uint32_t signature          = 0; // Identifier for the type.
        uint16_t diskIndex          = 0;
        uint16_t startDisk          = 0;
        uint16_t numberOfFiles      = 0; // How many files are in the zip file.
        uint16_t totalFiles         = 0; // This will be the same as numberOfFiles
        uint32_t dirSize            = 0; // How big is the directory
        uint32_t dirOffset          = 0; // Where does the directory actually start
        uint16_t commentLength      = 0;
    };
#pragma pack()

#pragma pack(1)
    struct FileHeader
    {
        // Initialize these all to 0.
        uint32_t signature          = 0; // Identifier for the type.
        uint16_t versionMade        = 0;
        uint16_t versionNeeded      = 0;
        uint16_t flag               = 0;
        uint16_t compression        = 0; // Type of compression. Can be 'no compression'
        uint16_t time               = 0;
        uint16_t date               = 0;
        uint32_t crcCode            = 0;
        uint32_t compressedSize     = 0; // Size of the compressed data
        uint32_t uncompressedSize   = 0; // Size of the original uncompressed file.
        uint16_t nameLength         = 0; // Length of the name of the file.
        uint16_t extraLength        = 0;
        uint16_t commentLength      = 0;
        uint16_t startDisk          = 0;
        uint16_t internalAttributes = 0;
        uint32_t externalAttributes = 0;
        uint32_t dataOffset         = 0; // Offset to the actual compressed data in the file.
    };
#pragma pack()

#pragma pack(1)
    struct DataHeader
    {
        uint32_t signature          = 0; // Identifier for the type.
        uint16_t version            = 0;
        uint16_t flag               = 0;
        uint16_t compression        = 0; // Type of compression. Can be 'no compression'
        uint16_t time               = 0;
        uint16_t date               = 0;
        uint32_t crcCode            = 0;
        uint32_t compressedSize     = 0; // Size of the compressed data
        uint32_t uncompressedSize   = 0; // Size of the original uncompressed file.
        uint16_t nameLength         = 0; // Length of the name of the file.
        uint16_t extraLength        = 0;
    };
#pragma pack()


    bool CompressFile(const char* pFileName, const char* pDestFileName);
    bool ExpandFile(const char* pFileName, const char* pDestFileName);
}