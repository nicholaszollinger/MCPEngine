#pragma once
// HighPrecisionTimer.h

#include <chrono>

class HighPrecisionTimer
{
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    using TimeDuration = std::chrono::duration<double>;

    TimePoint m_startTime; // Reference point for the timer.

public:
    HighPrecisionTimer() = default;

    void Start();
    double GetTimer() const;
};