// FrameTimer.cpp

#include "FrameTimer.h"

FrameTimer::FrameTimer()
{
    m_timer.Start();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Resets the internal timer to 0.
//-----------------------------------------------------------------------------------------------------------------------------
void FrameTimer::ResetTimer()
{
    m_timer.Start();
}

//-----------------------------------------------------------------------------------------------------------------------------
///		@brief : Begins a new frame, and returns the time in milliseconds since the last frame.
//-----------------------------------------------------------------------------------------------------------------------------
double FrameTimer::NewFrame()
{
    const double deltaTimeMs = m_timer.GetTimer();
    m_timer.Start();

    return deltaTimeMs;
}