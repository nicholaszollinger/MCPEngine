// AnimationCurve.cpp

#include "AnimationCurve.h"

AnimationCurve::AnimationCurve()
    : AnimationCurve(0.f, 1.f, {0.5f, 0.f}, {0.5f, 1.f})
{
    //
}

AnimationCurve::AnimationCurve(const float start, const float end, const Vec2& control1, const Vec2& control2)
    : m_start(kStartX, start)
    , m_end(kEndX, end)
    , m_controlPoint1(control1)
    , m_controlPoint2(control2)
    , m_timeElapsed(0.f)
    , m_duration(1.f) 
    , m_isComplete(false)
{
    //
}

void AnimationCurve::Begin()
{
    m_isComplete = false;
    m_timeElapsed = 0.f;
}

float AnimationCurve::Evaluate(const double deltaMs)
{
    if (m_isComplete)
    {
        return m_end.y;
    }

    m_timeElapsed += static_cast<float>(deltaMs);
    m_isComplete = m_timeElapsed > m_duration;

    const float t = m_timeElapsed / m_duration;

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