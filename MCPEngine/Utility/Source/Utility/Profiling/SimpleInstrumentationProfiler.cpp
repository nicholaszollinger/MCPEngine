// SimpleInstrumentationTimer.cpp

#include "SimpleInstrumentationProfiler.h"

#include <iostream>

#include "Logging/Log.h"

SimpleInstrumentationProfiler::SimpleInstrumentationProfiler(const char* label)
    : m_label(label)
{
    m_timer.Start();
}

SimpleInstrumentationProfiler::~SimpleInstrumentationProfiler()
{
    [[maybe_unused]] const double result = m_timer.GetTimer();
    
    // Log the time.
    _LOG("Profiler", "[", m_label, "] Result: " , result , "ms");
}