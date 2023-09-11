// Zip.cpp

#include "Zip.h"

#include <cassert>
#include <cstdio>
#include <string>
#define ZLIB_WINAPI
#include <zlib.h>
#include "MCP/Application/Debug/Log.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

using Byte = unsigned char;

// Buffer Size, as per the example. Is this number special? This was the CHUNK macro from the source.
static constexpr unsigned kBufferSize = 16384;

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      Source: https://www.zlib.net/zlib_how.html (Deflate function)
//
///		@brief : Compress a file and write the compressed data to a new file.
///		@param source : Source file that we are compressing.
///		@param dest : Destination file that we are writing the compressed data to.
///		@param level : Zlib compression level.
///		@returns : On success it returns Z_OK, otherwise a Zlib error code.
//-----------------------------------------------------------------------------------------------------------------------------
int Compress(FILE* source, FILE* dest, const int level)
{
    int flush{};
    unsigned have{};
    z_stream strm{};

    Byte* in = new Byte[kBufferSize];
    Byte* out = new Byte[kBufferSize];

    /* allocate deflate state */
    strm.zalloc = nullptr;
    strm.zfree = nullptr;
    strm.opaque = nullptr;
    int returnCode = deflateInit(&strm, level);
    if (returnCode != Z_OK)
        return returnCode;

    /* compress until end of file */
    do {
        strm.avail_in = static_cast<uInt>(fread(in, 1, kBufferSize, source));
        if (ferror(source))
        {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }

        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            strm.avail_out = kBufferSize;
            strm.next_out = out;
            returnCode = deflate(&strm, flush);    /* no bad return value */

            assert(returnCode != Z_STREAM_ERROR);  /* state not clobbered */
            have = kBufferSize - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest))
            {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);

    assert(returnCode == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&strm);

    delete[] in;
    delete[] out;

    return Z_OK;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      Source: https://www.zlib.net/zlib_how.html (Inflate function)
//
///		@brief : Expand a compressed file and write the expanded data to a new file.
///		@param source : Source file that we are expanding.
///		@param dest : Where we are going to write the expanded information.
///		@returns : On success it returns Z_OK, otherwise a Zlib error code.
//-----------------------------------------------------------------------------------------------------------------------------
int Expand(FILE* source, FILE* dest)
{
    unsigned have{};
    z_stream strm{};

    Byte* in = new Byte[kBufferSize];
    Byte* out = new Byte[kBufferSize];

    /* allocate inflate state */
    strm.zalloc = nullptr;
    strm.zfree = nullptr;
    strm.opaque = nullptr;
    strm.avail_in = 0;
    strm.next_in = nullptr;
    int ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = static_cast<uInt>(fread(in, 1, kBufferSize, source));
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }

        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = kBufferSize;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret)
            {
            case Z_NEED_DICT:
            {
                ret = Z_DATA_ERROR;
                [[fallthrough]];
            }

            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;

            default: break;
            }

            have = kBufferSize - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }

        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);

    delete[] in;
    delete[] out;

    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

//-----------------------------------------------------------------------------------------------------------------------------
///		@brief : Convert a Zlib error code into a string literal explaining the issue.
//-----------------------------------------------------------------------------------------------------------------------------
const char* GetZLibError(const int errorCode)
{
    switch (errorCode)
    {
        case Z_STREAM_ERROR: return "Invalid Compression Level.";
        case Z_DATA_ERROR: return "Invalid or incomplete deflate data.";
        case Z_MEM_ERROR: return "Out of Memory.";
        case Z_VERSION_ERROR: return "zlib version mismatch!";
        default: break;
    }

    return "Unknown Error.";
}

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Compress a file and write it to another file.
    ///		@param pFileName : Path of the file to compress.
    ///		@param pDestFileName : Path of the file to write the compressed data to.
    ///		@returns : True on success.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool CompressFile(const char* pFileName, const char* pDestFileName)
    {
        FILE* pSourceFile = nullptr;
        FILE* pDestFile = nullptr;

        if (fopen_s(&pSourceFile, pFileName, "r") != 0)
        {
            LogError("Failed to Compress file! Failed to open file for read at path: '%'", pFileName);
            return false;
        }

        if (fopen_s(&pDestFile, pDestFileName, "w") != 0)
        {
            LogError("Failed to Compress file! Failed to open file for write at path: '%'", pDestFileName);

            // Close the source file.
            if (fclose(pSourceFile) != 0)
                LogError("Failed to Compress file! Failed to close file at path: '%'", pFileName);

            return false;
        }

        if (const int code = Compress(pSourceFile, pDestFile, Z_DEFAULT_COMPRESSION); code != Z_OK)
        {
            LogError("Failed to Compress file! ZLib Error: '%'", GetZLibError(code));

            // Try to close the files.
            if (fclose(pSourceFile) != 0)
                LogError("Failed to Compress file! Failed to close file at path: '%'", pFileName);

            if (fclose(pDestFile) != 0)
                LogError("Failed to Compress file! Failed to close file at path: '%'", pDestFileName);

            return false;
        }

        if (fclose(pSourceFile) != 0)
        {
            LogError("Failed to Compress file! Failed to close file at path: '%'", pFileName);

            if (fclose(pDestFile) != 0)
                LogError("Failed to Compress file! Failed to close file at path: '%'", pDestFileName);

            return false;
        }

        if (fclose(pDestFile) != 0)
        {
            LogError("Failed to Compress file! Failed to close file at path: '%'", pDestFileName);
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Expand a compressed file and write it to another file.
    ///		@param pFileName : Path of the compressed file to expand.
    ///		@param pDestFileName : Path of the file to write the expanded data to.
    ///		@returns : True on success.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool ExpandFile(const char* pFileName, const char* pDestFileName)
    {
        FILE* pSourceFile = nullptr;
        FILE* pDestFile = nullptr;

        if (fopen_s(&pSourceFile, pFileName, "r") != 0)
        {
            LogError("Failed to Expand file! Failed to open file for read at path: '%'", pFileName);
            return false;
        }

        if (fopen_s(&pDestFile, pDestFileName, "w") != 0)
        {
            LogError("Failed to Expand file! Failed to open file for write at path: '%'", pDestFileName);

            // Close the Source file.
            if (fclose(pSourceFile) != 0)
                LogError("Failed to Expand file! Failed to close file at path: '%'", pDestFileName);

            return false;
        }

        if (const int code = Expand(pSourceFile, pDestFile); code != Z_OK)
        {
            LogError("Failed to Expand file! ZLib Error: '%'", GetZLibError(code));

            // Try to close the files.
            if (fclose(pSourceFile) != 0)
                LogError("Failed to Expand file! Failed to close file at path: '%'", pFileName);

            if (fclose(pDestFile) != 0)
                LogError("Failed to Expand file! Failed to close file at path: '%'", pDestFileName);
            

            return false;
        }

        if (fclose(pSourceFile) != 0)
        {
            LogError("Failed to Expand file! Failed to close file at path: '%'", pFileName);

            if (fclose(pDestFile) != 0)
                LogError("Failed to Expand file! Failed to close file at path: '%'", pDestFileName);

            return false;
        }

        if (fclose(pDestFile) != 0)
        {
            LogError("Failed to Expand file! Failed to close file at path: '%'", pDestFileName);
            return false;
        }

        return true;
    }

}