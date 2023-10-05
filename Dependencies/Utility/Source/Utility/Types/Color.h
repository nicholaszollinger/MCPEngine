#pragma once
// Color.h

#include <cstdint>

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t alpha;

    constexpr Color()
        : r(255)
        , g(255)
        , b(255)
        , alpha(255)
    {
        // Default to white.
    }

    constexpr Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t alpha = 255)
        : r(r)
        , g(g)
        , b(b)
        , alpha(alpha)
    {
        //
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Colors are equal if their r, g, b values are equal. We don't care about the alpha.
    //-----------------------------------------------------------------------------------------------------------------------------
    constexpr bool operator==(const Color& other) const
    {
        return r == other.r && g == other.g && b == other.b;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Colors are not equal if their r, g, b values are not equal. We don't care about the alpha.
    //-----------------------------------------------------------------------------------------------------------------------------
    constexpr bool operator!=(const Color& other) const
    {
        return !(*this == other);
    }

    static constexpr Color White() { return {255,255,255,255}; }
    static constexpr Color Black() { return {0,0,0,255}; }
};