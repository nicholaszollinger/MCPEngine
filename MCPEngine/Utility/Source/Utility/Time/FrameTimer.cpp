// FrameTimer.cpp

#include "FrameTimer.h"

#include <chrono>

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Sets the 'last frame' to the current time. Use this to 'start' the timer.
//-----------------------------------------------------------------------------------------------------------------------------
void FrameTimer::Init()
{
    using namespace std::chrono;
    using Duration = duration<long long, std::micro>;

    m_lastFrameTime = time_point_cast<Duration>(high_resolution_clock::now()).time_since_epoch().count();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Returns the amount of time in seconds since the 'last frame'. It then sets the current time to the new
///         'last frame'.
//-----------------------------------------------------------------------------------------------------------------------------
float FrameTimer::NewFrame()
{
    using namespace std::chrono;
    using namespace std::chrono_literals;
    using Duration = duration<long long, std::micro>;

    // Get the current time,
    const auto newFrameTime = time_point_cast<Duration>(high_resolution_clock::now());

    // Cast the old frame time as a chrono object that matches the duration.
    const time_point<high_resolution_clock, Duration> lastFrame{ Duration{m_lastFrameTime} };

    // Compute the time that has elapsed between them.
    const Duration timeElapsed = newFrameTime - lastFrame;

    // Update our last frame to the new frame.
    m_lastFrameTime = newFrameTime.time_since_epoch().count();

    // Return the time as a float in seconds.
    return static_cast<float>(timeElapsed.count()) / 1000000.f;
}