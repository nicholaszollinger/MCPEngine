#pragma once
// SimpleInstrumentationProfiler.h

// Set this to '1' if you want to enable profiling. Setting to 0 will turn it off.
#define ENABLE_SIMPLE_PROFILER 1
#include <string>

#if ENABLE_SIMPLE_PROFILER
#include "../Time/HighPrecisionTimer.h"

class SimpleInstrumentationProfiler
{
    HighPrecisionTimer m_timer;
    std::string m_label;

public:
    SimpleInstrumentationProfiler(const char* label);
    ~SimpleInstrumentationProfiler();
};

#define START_PROFILER(str) SimpleInstrumentationProfiler profiler(str)

#else
#define START_PROFILER(str)
#endif