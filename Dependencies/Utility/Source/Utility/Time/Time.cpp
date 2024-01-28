// Time.cpp

#include "Time.h"

#include <chrono>
#include <ctime>

namespace Time
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Get the current Date and time in the form of a long long.
    //-----------------------------------------------------------------------------------------------------------------------------
    TimeStamp GetCurrentDateAndTime()
    {
        const auto now = std::chrono::system_clock::now();
        return std::chrono::system_clock::to_time_t(now);
    }

    tm GetCurrentTimeInfo()
    {
        const TimeStamp now = GetCurrentDateAndTime();
        tm timeInfo{};
        [[maybe_unused]] auto pInfo = localtime_s(&timeInfo, &now);

        return timeInfo;
    }

    float GetDeltaTime(const TimeStamp lastTimeRecorded)
    {
        std::chrono::duration<float> timeElapsed(GetCurrentDateAndTime() - lastTimeRecorded);
        return timeElapsed.count();
    }
}