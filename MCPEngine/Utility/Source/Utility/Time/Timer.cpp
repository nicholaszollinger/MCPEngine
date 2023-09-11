// Timer.cpp

#include "Timer.h"
#include <chrono>
 
//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      I put this as a global in the source file because I didn't want the header to contain <chrono>
//
///		@brief : Duration type to be used for reading the system clock.
//-----------------------------------------------------------------------------------------------------------------------------
using Duration = std::chrono::duration<long long, std::ratio<1, 10000000>>;

Timer::Timer()
    : m_elapsedTime()
    , m_startTime()
{
    //
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Resets and starts the timer.
//-----------------------------------------------------------------------------------------------------------------------------
void Timer::Start()
{
    m_elapsedTime = 0;

    auto now = std::chrono::time_point_cast<Duration>(std::chrono::system_clock::now());
    m_startTime = now.time_since_epoch().count();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Start the timer with an initial duration.
//-----------------------------------------------------------------------------------------------------------------------------
void Timer::Start(const long long duration)
{
    m_elapsedTime = duration;

    auto now = std::chrono::time_point_cast<Duration>(std::chrono::system_clock::now());
    m_startTime = now.time_since_epoch().count();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Returns the duration from the Start call to now.
//-----------------------------------------------------------------------------------------------------------------------------
long long Timer::GetElapsedTime()
{
    auto end = std::chrono::time_point_cast<Duration>(std::chrono::system_clock::now());
    std::chrono::time_point<std::chrono::system_clock, Duration> prev{ Duration{m_startTime} };
    m_elapsedTime += (end - prev).count();

    return m_elapsedTime;
}