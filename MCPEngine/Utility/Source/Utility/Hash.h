#pragma once
// Hash.h

#include <cstdint>

namespace Internal
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Ref: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    //
    ///		@brief : Generate a uint32_t number based on a string using the fnv1a_32 hash.
    //-----------------------------------------------------------------------------------------------------------------------------
    inline constexpr uint32_t StringFnv1a32Hash(const char* str, const size_t count)
    {
        return count != 0 ? (StringFnv1a32Hash(str, count - 1) ^ str[count - 1]) * 16777619u : 2166136261u; 
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Calculate the size of a c-style string at compile time.
    //-----------------------------------------------------------------------------------------------------------------------------
    constexpr size_t StrLength(const char* str)
    {
        return *str ? 1 + StrLength(str + 1) : 0;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Generate a uint32_t number based on a string.
//-----------------------------------------------------------------------------------------------------------------------------
constexpr uint32_t HashString(const char* str)
{
    return Internal::StringFnv1a32Hash(str, Internal::StrLength(str));
}