#pragma once
// Vector2.h

#include <cmath>
#include <type_traits>

#include "FloatTester.h"

template<typename Type>
struct Vector2
{
    static_assert(std::is_integral_v<Type> || std::is_floating_point_v<Type> && "Type must be an integral or floating-point type!");

    Type x;
    Type y;

    constexpr Vector2()
        : x{}   // Should default to zero.
        , y{}   // Should default to zero.
    {
        //
    }

    constexpr Vector2(const Type x, const Type y)
        : x(x)
        , y(y)
    {
        //
    }

    constexpr Vector2(const Vector2& right)
        : x(right.x)
        , y(right.y)
    {
        //
    }
    
    constexpr Vector2& operator=(const Vector2& right)
    {
        x = right.x;
        y = right.y;

        return *this;
    }

    ~Vector2() = default;

    Vector2 GenerateUnitVector()
    {
        return Vector2{ static_cast<Type>(1), static_cast<Type>(1), static_cast<Type>(1) };
    }

    float GetMagnitude() const
    {
        auto sum = (x * x) + (y * y);
        return std::sqrt(sum);
    }

    Type GetSquaredMagnitude() const
    {
        return (x * x) + (y * y);
    }

    Type GetDotProduct(const Vector2 right) const
    {
        return  (x * right.x) + (y * right.y);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      I move the normalized functions outside of the object because the result of normalization should be a floating-point
    //      typed vector.
    //
    ///		@brief : Normalizes THIS vector! If you want to get a normalized version without changing this vector, use
    ///             'GetNormalized()'.
    //-----------------------------------------------------------------------------------------------------------------------------
    //void Normalize()
    //{
    //    auto magnitude = GetMagnitude();
    //    
    //    // If our magnitude is zero, then return immediately.
    //    if (CheckEqualFloats(magnitude, 0.0f, 0.0001f)) 
    //        return;

    //    x = x / magnitude;
    //    y = y / magnitude;
    //}

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      I move the normalized functions outside of the object because the result of normalization should be a floating-point
    //      typed vector.
    //
    ///		@brief : Returns a normalized Vector2 based on this vector's values. NOTE: This does not modify this vector.
    //-----------------------------------------------------------------------------------------------------------------------------
    /*Vector2 GetNormalized() const
    {
        Vector2 output = *this;
        output.Normalize();
        return output;
    }*/

    bool operator==(const Vector2& right) const
    {
        if constexpr (std::is_floating_point_v<Type>)
        {
            return CheckEqualFloats(x, right.x)
            && CheckEqualFloats(y, right.y);
        }

        return x == right.x && y == right.y;
    }

    Vector2 operator+(const Vector2<Type>& right) const
    {
        Vector2 result = right;
        result.x += x;
        result.y += y;
        return result;
    }

    Vector2& operator+=(const Vector2<Type>& right)
    {
        *this = *this + right;
        return *this;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Calculates a Vector that points from 'this' to 'right'.
    //-----------------------------------------------------------------------------------------------------------------------------
    constexpr Vector2 operator-(const Vector2<Type>& right) const
    {
        Vector2 result = *this;
        result.x -= right.x;
        result.y -= right.y;
        return result;
    }

    Vector2 operator-() const
    {
        Vector2 result;
        result.x = -x;
        result.y = -y;

        return result;
    }

    Vector2& operator-()
    {
        x = -x;
        y = -y;

        return *this;
    }

    Vector2& operator-=(const Vector2& right)
    {
        *this = *this - right;
        return *this;
    }

    Vector2 operator*(const Vector2& right) const
    {
        Vector2 result = right;
        result.x *= x;
        result.y *= y;
        return result;
    }

    template<typename ScalarType>
    Vector2 operator*(const ScalarType right) const
    {
        static_assert(std::is_integral_v<ScalarType> || std::is_floating_point_v<ScalarType> && "Scalar Type must be an integral or floating-point type!");

        Vector2 result = *this;
        result.x *= static_cast<Type>(right);
        result.y *= static_cast<Type>(right);
        return result;
    }

    template<typename ScalarType>
    Vector2& operator*=(const ScalarType right)
    {
        static_assert(std::is_integral_v<ScalarType> || std::is_floating_point_v<ScalarType> && "Scalar Type must be an integral or floating-point type!");

        *this = *this * right;
        return *this;
    }

    template<typename ScalarType>
    Vector2 operator/(const ScalarType right) const
    {
        static_assert(std::is_integral_v<ScalarType> || std::is_floating_point_v<ScalarType> && "Scalar Type must be an integral or floating-point type!");

        Vector2 result = *this;
        result.x /= static_cast<Type>(right);
        result.y /= static_cast<Type>(right);
        return result;
    }

    template<typename ScalarType>
    Vector2& operator/=(const ScalarType right)
    {
        static_assert(std::is_integral_v<ScalarType> || std::is_floating_point_v<ScalarType> && "Scalar Type must be an integral or floating-point type!");

        *this = *this / right;
        return *this;
    }
};

template<typename Type, typename ScalarType>
Vector2<Type> operator*(const float scalar, const Vector2<Type>& vec)
{
    return vec * scalar;
}

template<typename Type>
Type GetSquaredDistance(const Vector2<Type>& left, const Vector2<Type>& right)
{
    const Type xDiff = right.x - left.x;
    const Type yDiff = right.y - left.y;

    return (xDiff) * (xDiff) + (yDiff) * (yDiff);
}

template<typename Type>
float GetDistance(const Vector2<Type>& left, const Vector2<Type>& right)
{
    return std::sqrt(GetSquaredDistance(left, right));
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Normalizes the vector. \n NOTE: If you want to get a normalized version without changing this vector, use
///             'GetNormalized()'.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename Type>
void Normalize(Vector2<Type>& vec)
{
    static_assert(std::is_floating_point_v<Type> && "Normalizing Vectors should be done on floating-point typed Vectors!");

    auto magnitude = vec.GetMagnitude();

    // If our magnitude is zero, then return immediately.
    if (CheckEqualFloats(magnitude, 0.f))
        return;

    vec.x = vec.x / magnitude;
    vec.y = vec.y / magnitude;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Returns a normalized Vector2 based on this vector's values. NOTE: This does not modify this vector.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename Type>
Vector2<float> GetNormalized(const Vector2<Type>& vec)
{
    Vector2<float> output;
    output.x = static_cast<float>(vec.x);
    output.y = static_cast<float>(vec.y);

    Normalize(output);
    return output;
}

using Vec2 = Vector2<float>;
using Vec2Int = Vector2<int>;

constexpr Vector2<float> kVec2Zero{ 0.f, 0.f };
constexpr Vector2<float> kVec2Up{ 0.f, 1.f };
constexpr Vector2<float> kVec2Right{ 1.f, 0.f };