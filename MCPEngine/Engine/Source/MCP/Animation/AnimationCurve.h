#pragma once
// AnimationCurve.h

#include <functional>
#include "ConceptTypes.h"
#include "Vector2.h"

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		The goal of this class is to create a curve to 'animate' a certain value. E.g. For an image fade in, we might want to animate the
//      alpha value by 'easing-in' to full opacity.
//
//      A curve is going to be defined as moving from (0.f, yValue) -> (1.f , yValue)
//
//      Evaluate is always going to return a normalized value [0.f , 1.f]. This value that you get from the curve can
//      be multiplied by the value of whatever you are animating. So, color.alpha * Evaluate(deltaMs) would give you the
//      current alpha value based on the Animation Curve that was set.
//
//      Source for math: https://www.gatevidyalay.com/bezier-curve-in-computer-graphics-examples/
//      
///		@brief : 
//-----------------------------------------------------------------------------------------------------------------------------
class AnimationCurve
{ 
public:
   AnimationCurve();
   AnimationCurve(const float start, const float end, const Vec2& control1, const Vec2& control2);

    void Begin();
    float Evaluate(const double deltaMs);
    void SetDuration(const float duration) { m_duration = duration; }
    [[nodiscard]] bool IsComplete() const { return m_isComplete; }

private:
    static constexpr float kStartX = 0.f;
    static constexpr float kEndX = 1.f;

    const Vec2 m_start;
    const Vec2 m_end;
    const Vec2 m_controlPoint1;
    const Vec2 m_controlPoint2;
    float m_timeElapsed;
    float m_duration;
    bool m_isComplete;
};

template<typename ValueType>
class Curve
{
    static_assert(TypesAreMultipliable<ValueType, float>::value, "ValueType must be multipliable with a float value!");

public:
    using WeightAverageFunc = std::function<ValueType(const float, const ValueType&, const ValueType&)>;

public:
    Curve()
        : Curve(0.f, 1.f, { 0.5f, 0.f }, { 0.5f, 1.f })
    {
        //
    }

    Curve(const float start, const float end, const Vec2& control1, const Vec2 control2)
        :m_start(kStartX, start)
        , m_end(kEndX, end)
        , m_controlPoint1(control1)
        , m_controlPoint2(control2)
        , m_weightedAverageFunc([this](const float w, const ValueType& startVal, const ValueType& endVal)->ValueType { return this->GetWeightedAverage(w, startVal, endVal); })
    {
        //
    }

    ValueType Evaluate(const float t, const ValueType& initialVal, const ValueType& endVal) const
    {
        const float result = GetCurveResult(t);

        if constexpr (std::is_integral_v<ValueType>)
        {
            return static_cast<ValueType>(static_cast<float>((endVal - initialVal)) * result + static_cast<float>(initialVal));
        }

        else
        {
            return m_weightedAverageFunc(result, initialVal, endVal);
        }
    }

    void SetControlPoint1(const Vec2& point)
    {
        m_controlPoint1 = point;
    }

    void SetControlPoint2(const Vec2& point)
    {
        m_controlPoint2 = point;
    }

    void SetEndInfluence(const Vec2& endPoint)
    {
        m_end = endPoint;
    }

    void SetWeightedAverageFunction(const WeightAverageFunc& func)
    {
        m_weightedAverageFunc = func;
    }

private:
    static constexpr float kStartX = 0.f;
    static constexpr float kEndX = 1.f;

    WeightAverageFunc m_weightedAverageFunc;
    Vec2 m_start;
    Vec2 m_end;
    Vec2 m_controlPoint1;
    Vec2 m_controlPoint2;

    ValueType GetWeightedAverage(const float weight, const ValueType& initialVal, const ValueType& endVal) const
    {
        // (e_1 - e_0)
        ValueType output = endVal - initialVal;
        output = static_cast<ValueType>(output * weight);
        return output + initialVal;
    }

    [[nodiscard]] float GetCurveResult(const float t) const
    {
        const float t2 = t * t;
        const float t3 = t2 * t;

        // a = (1 - t)^3
        const float a = -t3 + 3 * t2 - 3 * t + 1;

        // b = 3t(1 - t)^2
        const float b = 3 * t * (t2 - 2 * t + 1);

        // c = 3t^2(1 - t)
        const float c = 3 * t2 * (1 - t);

        // d = t^3 = t3
        const Vec2 result = m_start * a + m_controlPoint1 * b + m_controlPoint2 * c + m_end * t3;

        return result.y;
    }
};

inline AnimationCurve g_easeIn(0.f, 1.f, { 0.5f, 1.f }, { 0.5f, 1.f });
inline AnimationCurve g_easeOut(1.f, 0.f, { 0.5f, 0.f }, { 0.5f, 0.f });