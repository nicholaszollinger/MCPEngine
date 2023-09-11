#pragma once
// Rect.h

#include <algorithm>
#include "Vector2.h"

template<typename Type>
struct Rect
{
    constexpr Rect()
        : width()
        , height()
    {
        //
    }

    constexpr Rect(const Type x, const Type y, const Type width, const Type height)
        : position(x, y)
        , width(width)
        , height(height)
    {
        //
    }

    Vector2<Type> position;
    Type width;
    Type height;

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Checks to see if this rect is completely inside the 'outer' rect, meaning that there are no points
    ///             of the this rect that are outside the outer rect.
    //-----------------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] bool IsInside(const Rect& outer) const
    {
        return outer.position.x < position.x
            && outer.position.x + outer.width > position.x + width
            && outer.position.y < position.y
            && outer.position.y + outer.height > position.y + height;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Calculates the intersect of this and another Rect as another rect. If no collision was
    ///             made, then the width and height would be 0.
    //-----------------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] Rect GetIntersectionAsRect(const Rect& other) const
    {
        Rect result;
        result.position.x = std::max(position.x, other.position.x);
        result.position.y = std::max(position.y, other.position.y);
        result.width = std::min(position.x + width, other.position.x + other.width) - result.position.x;
        result.height = std::min(position.y + height, other.position.y + other.height) - result.position.y;

        return result;
    }

    [[nodiscard]] bool HasValidDimensions() const
    {
        return width > static_cast<Type>(0) && height > static_cast<Type>(0);
    }
};

using RectF = Rect<float>;
using RectInt = Rect<int>;

template<typename IntegralType>
inline RectF ToFloat(const Rect<IntegralType>& rect)
{
    return RectF( static_cast<float>(rect.position.x)
        , static_cast<float>(rect.position.y) 
        , static_cast<float>(rect.width) 
        , static_cast<float>(rect.height));
}

template<typename LeftRectType, typename RightRectType>
bool DoIntersect(const Rect<LeftRectType>& left, const Rect<RightRectType> right)
{
    // If there is a integral/floating point mismatch, promote the integral type to the floating point type.
    if constexpr (std::is_floating_point_v<LeftRectType> && std::is_integral_v<RightRectType>)
    {
        return DoIntersect(left, ToFloat<RightRectType>(right));
    }

    if constexpr (std::is_floating_point_v<RightRectType> && std::is_integral_v<LeftRectType>)
    {
        return DoIntersect(ToFloat<LeftRectType>(left), right);
    }

    return left.position.x < right.position.x + right.width
        && left.position.x + left.width > right.position.x
        && left.position.y < right.position.y + right.height
        && left.position.y + left.height > right.position.y;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Checks to see if the 'inner' rect is completely inside the 'outer' rect, meaning that there are no points
///             of the inner rect that are outside the outer rect.
//-----------------------------------------------------------------------------------------------------------------------------
inline bool IsInside(const RectF& inner, const RectF& outer)
{
    return outer.position.x <= inner.position.x
            && outer.position.x + outer.width >= inner.position.x + inner.width
            && outer.position.y <= inner.position.y
            && outer.position.y + outer.height >= inner.position.y + inner.height;
}