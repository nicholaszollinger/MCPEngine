#pragma once
// Color.h

struct Color
{
    using Uint8 = unsigned char;

    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 alpha;

    constexpr Color()
        : r(255)
        , g(255)
        , b(255)
        , alpha(255)
    {
        // Default to white.
    }

    constexpr Color(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 alpha = 255)
        : r(r)
        , g(g)
        , b(b)
        , alpha(alpha)
    {
        //
    }
};