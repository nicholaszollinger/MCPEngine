#pragma once
// Time.h

#include <string>

using TimeStamp = long long;

TimeStamp GetCurrentDateAndTime();
tm GetCurrentTimeInfo();

float GetDeltaTime(const TimeStamp lastTimeRecorded);