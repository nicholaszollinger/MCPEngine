#pragma once
// Bit.h
// Goal: Global functions for bit manipulation:

#include <type_traits>

template<typename IntegralType>
constexpr IntegralType Bit(const unsigned n)
{
    return static_cast<IntegralType>(1 << n);
}

template<typename IntegralType>
constexpr bool IsBitSet(const IntegralType& number, const int n)
{
    static_assert(std::is_integral_v<IntegralType>, "IntegralType must be an integral type!");

    return number & (1 << n);
}

template<typename IntegralType>
constexpr void SetBit(IntegralType& number, const int n)
{
    static_assert(std::is_integral_v<IntegralType>, "IntegralType must be an integral type!");

    number |= (1 << n);
}

template<typename IntegralType>
constexpr void ClearBit(IntegralType& number, const int n)
{
    static_assert(std::is_integral_v<IntegralType>, "IntegralType must be an integral type!");
    number &= ~(1 << n);
}

template<typename IntegralType>
constexpr void ToggleBit(IntegralType& number, const int n)
{
    static_assert(std::is_integral_v<IntegralType>, "IntegralType must be an integral type!");
    number ^= (1 << n);
}