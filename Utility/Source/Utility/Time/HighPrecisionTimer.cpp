// HighPrecisionTimer.cpp

#include "HighPrecisionTimer.h"

//-----------------------------------------------------------------------------------------------------------------------------
///		@brief : Start the timer.
//-----------------------------------------------------------------------------------------------------------------------------
void HighPrecisionTimer::Start()
{
    m_startTime = std::chrono::high_resolution_clock::now();
}

//-----------------------------------------------------------------------------------------------------------------------------
///		@brief : Get the time (in milliseconds) has elapsed since the last StartTimer() call.
//-----------------------------------------------------------------------------------------------------------------------------
double HighPrecisionTimer::GetTimer() const
{
    const TimePoint endTime = std::chrono::high_resolution_clock::now();
    const TimeDuration timeSpan = std::chrono::duration_cast<TimeDuration>(endTime - m_startTime);

    return timeSpan.count() * 1000; // Convert to milliseconds before returning.
}