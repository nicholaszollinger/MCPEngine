#pragma once
// Rect.h

#include <algorithm>
#include "Vector2.h"

template<typename Type>
struct Rect
{
    static_assert(std::is_integral_v<Type> || std::is_floating_point_v<Type>, "Rect Type must be an integral or floating point type!");

    Type x;
    Type y;
    Type width;
    Type height;

    constexpr Rect() = default;
    constexpr Rect(const Type x, const Type y, const Type width, const Type height)
        : x(x)
        , y(y)
        , width(width)
        , height(height)
    {
        //
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Return the x and y values of the Rect as a Vector2.
    //-----------------------------------------------------------------------------------------------------------------------------
    constexpr Vector2<Type> GetPosition() const
    {
        return Vector2<Type>(x, y);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Set the x and y values of the Rect using a Vector2.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SetPosition(const Vector2<Type> pos)
    {
        SetPosition(pos.x, pos.y);
    }

    void SetPosition(const Type _x, const Type _y)
    {
        x = _x;
        y = _y;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Returns true if the 'other' Rect is intersecting with this Rect.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename OtherRectType>
    [[nodiscard]] constexpr bool Intersects(const Rect<OtherRectType>& other) const
    {
        static_assert(std::is_integral_v<OtherRectType> || std::is_floating_point_v<OtherRectType>, "Rect Type must be an integral or floating point type!");

        // If this type is integral and the other is floating point, promote this type to floating
        // point and check the intersection.
        if constexpr (std::is_integral_v<Type> && std::is_floating_point_v<OtherRectType>)
        {
            return other.Intersects(GetRectAs<OtherRectType>());
        }

        // If this type is floating point and the other is integral, then we need to promote the
        // other type to floating point precision and check.
        if constexpr (std::is_floating_point_v<Type> && std::is_integral_v<OtherRectType>)
        {
            return Intersects(other.GetRectAs<Type>());
        }

        return x < other.x + other.width
            && x + width > other.x
            && y < other.y + other.height
            && y + height > other.y;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns true if the point is inside our rect.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename Vec2Type>
    [[nodiscard]] constexpr bool Intersects(const Vector2<Vec2Type> point) const
    {
        // TODO: I need to make sure that I cast appropriately.

        return x < point.x
            && x + width > point.x
            && y < point.y
            && y + height > point.y;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Calculates the intersect of this and another Rect as another rect. If there is no intersection,
    ///              then the width and height would be 0.
    //-----------------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] constexpr Rect GetIntersectionAsRect(const Rect& other) const
    {
        Rect result;
        result.x = std::max(x, other.x);
        result.y = std::max(y, other.y);
        result.width = std::min(x + width, other.x + other.width) - result.x;
        result.height = std::min(y + height, other.y + other.height) - result.y;

        return result;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns true if both width and height are greater than zero. 
    //-----------------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] constexpr bool HasValidDimensions() const
    {
        return width > static_cast<Type>(0) && height > static_cast<Type>(0);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns a copy of this rect converted to another type.
    ///		@tparam OtherType : The Type you want the copy to be.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename OtherType>
    [[nodiscard]] constexpr Rect<OtherType> GetRectAs() const
    {
        if constexpr (std::is_same_v<OtherType, Type>)
        {
            return *this;
        }

        Rect<OtherType> result;
        result.x = static_cast<OtherType>(x);
        result.y = static_cast<OtherType>(y);
        result.width = static_cast<OtherType>(width);
        result.height = static_cast<OtherType>(height);

        return result;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Checks to see if this rect is completely inside the 'outer' rect, meaning that there are no points
    ///             of the this rect that are outside the outer rect.
    //-----------------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] bool IsInside(const Rect& outer) const
    {
        return outer.x < x
            && outer.x + outer.width > x + width
            && outer.y < y
            && outer.y + outer.height > y + height;
    }

    std::string ToString() const
    {
        return CombineIntoString("(x=", x, ", y=" , y, ", width=", width, ", height=", height, ")");
    }
};

using RectF = Rect<float>;
using RectInt = Rect<int>;