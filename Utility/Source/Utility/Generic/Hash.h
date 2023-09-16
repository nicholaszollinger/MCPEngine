#pragma once
// Hash.h

#include <cstdint>
#include <type_traits>

namespace Internal
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Algorithm:                              https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    //      InitialHash and PrimeMultiplier Values: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV_hash_parameters
    //		
    ///		@brief : Generate an integral value based off a c-style string. Note: The return type is matched to the initial hash's type.
    ///		@param pStr : String we are hashing from.
    ///		@param initialHash : The initial 'offset' of the hash. This is the 'offset basis' value in the Fnv1a documentation. 
    ///		@param primeMultiplier : A prime number we multiply by. This is the 'FNV prime' value in the Fnv1a documentation.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename UnsignedIntegralType>
    constexpr UnsignedIntegralType Fnv1aHashString(const char* pStr, const UnsignedIntegralType initialHash, const UnsignedIntegralType primeMultiplier)
    {
        static_assert(std::is_integral_v<UnsignedIntegralType> && std::is_unsigned_v<UnsignedIntegralType>, "Initial hash and prime must be unsigned integral types!");

        // If the string is nullptr, return the initial hash
        if (!pStr)
            return initialHash;

        auto hash = initialHash;

        while(*pStr != '\0')
        {
            hash ^= static_cast<UnsignedIntegralType>(*pStr);
            hash *= primeMultiplier;
            ++pStr;
        }

        return hash;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      InitialHash and PrimeMultiplier Values: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV_hash_parameters
//
///		@brief : Generate a uint32_t number based on a c-string.
//-----------------------------------------------------------------------------------------------------------------------------
constexpr uint32_t HashString32(const char* pStr)
{
    constexpr uint32_t kInitialHash = 0x811c9dc5;
    constexpr uint32_t kPrimeMultiplier = 0x01000193;

    return Internal::Fnv1aHashString(pStr, kInitialHash, kPrimeMultiplier);
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      InitialHash and PrimeMultiplier Values: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV_hash_parameters
//
///		@brief : Generate a uint364_t number based on a c-string.
//-----------------------------------------------------------------------------------------------------------------------------
constexpr uint64_t HashString64(const char* pStr)
{
    constexpr uint64_t kInitialHash = 0xcbf29ce484222325ull;
    constexpr uint64_t kPrimeMultiplier = 0x100000001b3ull;

    return Internal::Fnv1aHashString(pStr, kInitialHash, kPrimeMultiplier);
}