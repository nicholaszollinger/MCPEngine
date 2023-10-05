#pragma once
// Vector2.h

#include <cassert>
#include <cmath>
#include <type_traits>

#include "../Math/FloatTester.h"
#include "../String/FormatString.h"

template<typename Type>
struct Vector2
{
    static_assert(std::is_integral_v<Type> || std::is_floating_point_v<Type> && "Type must be an integral or floating-point type!");

    Type x;
    Type y;

    // Constructors, Assignment, Move, Destructor

    constexpr Vector2()
        : x{}
        , y{}
    {
        //
    }

    constexpr Vector2(const Type x, const Type y)
        : x(x)
        , y(y)
    {
        //
    }

    constexpr Vector2(const Vector2& right) = default;
    constexpr Vector2(Vector2&& right) noexcept = default;
    constexpr Vector2& operator=(const Vector2& right) = default;
    constexpr Vector2& operator=(Vector2&& right) noexcept = default;
    ~Vector2() = default;

    // Helpful functions

    [[nodiscard]] float GetMagnitude() const
    {
        auto sum = (x * x) + (y * y);
        return std::sqrt(sum);
    }

    [[nodiscard]] constexpr float GetSquaredMagnitude() const
    {
        return static_cast<float>((x * x) + (y * y));
    }

    [[nodiscard]] constexpr float GetDotProduct(const Vector2 right) const
    {
        return static_cast<float>((x * right.x) + (y * right.y));
    }

    Vector2 GenerateUnitVector()
    {
        return Vector2{ static_cast<Type>(1), static_cast<Type>(1), static_cast<Type>(1) };
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      I move the normalized functions outside of the object because the result of normalization should be a floating-point
    //      typed vector.
    //
    ///		@brief : Normalizes THIS vector! If you want to get a normalized version without changing this vector, use
    ///             'GetNormalized()'.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Normalize()
    {
        static_assert(std::is_floating_point_v<Type>, "Vector must have floating point data type to be Normalized!");

        auto magnitude = GetMagnitude();
        
        // If our magnitude is zero, then return immediately.
        if (CheckEqualFloats(magnitude, 0.0f, 0.0001f)) 
            return;

        x = x / magnitude;
        y = y / magnitude;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      I move the normalized functions outside of the object because the result of normalization should be a floating-point
    //      typed vector.
    //
    ///		@brief : Returns a normalized Vector2 based on this vector's values. NOTE: This does not modify this vector.
    //-----------------------------------------------------------------------------------------------------------------------------
    Vector2 GetNormalized() const
    {
        static_assert(std::is_floating_point_v<Type>, "Vector must have floating point data type to get Normalized version!");

        Vector2 output = *this;
        output.Normalize();
        return output;
    }

    // Operators

    // Disable the 'unreachable code' warning.
#pragma warning (push)
#pragma warning (disable : 4702)

    bool operator==(const Vector2& right) const
    {
        if constexpr (std::is_floating_point_v<Type>)
        {
            return CheckEqualFloats(x, right.x)
            && CheckEqualFloats(y, right.y);
        }

        return x == right.x && y == right.y;
    }

#pragma warning (pop)

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

    constexpr Vector2 operator-() const
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
    Vector2 operator*(const ScalarType scalar) const
    {
        static_assert(std::is_integral_v<ScalarType> || std::is_floating_point_v<ScalarType> && "Scalar Type must be an integral or floating-point type!");

        Vector2 result = *this;
        result.x *= static_cast<Type>(scalar);
        result.y *= static_cast<Type>(scalar);
        return result;
    }

    template<typename ScalarType>
    Vector2& operator*=(const ScalarType scalar)
    {
        static_assert(std::is_integral_v<ScalarType> || std::is_floating_point_v<ScalarType> && "Scalar Type must be an integral or floating-point type!");

        *this = *this * scalar;
        return *this;
    }

    template<typename ScalarType>
    Vector2 operator/(const ScalarType scalar) const
    {
        static_assert(std::is_integral_v<ScalarType> || std::is_floating_point_v<ScalarType> && "Scalar Type must be an integral or floating-point type!");

        // Assert that the value isn't zero.
        assert(scalar != static_cast<decltype(scalar)>(0) && "Cannot divide by zero!");

        Vector2 result = *this;
        result.x /= static_cast<Type>(scalar);
        result.y /= static_cast<Type>(scalar);
        return result;
    }

    template<typename ScalarType>
    Vector2& operator/=(const ScalarType scalar)
    {
        static_assert(std::is_integral_v<ScalarType> || std::is_floating_point_v<ScalarType> && "Scalar Type must be an integral or floating-point type!");

        *this = *this / scalar;
        return *this;
    }
    
    std::string ToString() const
    {
        return CombineIntoString("(", x, ", " , y, ")");
    }

    // Common Values.
    static constexpr Vector2 ZeroVector() { return Vector2(0,0); }
    static constexpr Vector2 UpVector() { return Vector2(0, static_cast<Type>(1)); }
    static constexpr Vector2 DownVector() { return Vector2(0, static_cast<Type>(-1)); }
    static constexpr Vector2 RightVector() { return Vector2(static_cast<Type>(1), 0); }
    static constexpr Vector2 LeftVector() { return Vector2(static_cast<Type>(-1), 0); }
};

using Vec2 = Vector2<float>;
using Vec2Int = Vector2<int>;

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