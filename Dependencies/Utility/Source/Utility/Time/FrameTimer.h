#pragma once
// FrameTimer.h
#include "HighPrecisionTimer.h"

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Wraps the HighPrecisionTimer in a frame time API. You simply create this timer on the stack (its constructor
///         will start the timer) then call NewFrame() at the top of the main loop to get the delta time for that frame.
//-----------------------------------------------------------------------------------------------------------------------------
class FrameTimer
{
    HighPrecisionTimer m_timer;

public:
    FrameTimer();

    void ResetTimer();
    double NewFrame();
};