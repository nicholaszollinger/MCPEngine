// SimpleInstrumentationTimer.cpp

#include "SimpleInstrumentationProfiler.h"

#include <iostream>

SimpleInstrumentationProfiler::SimpleInstrumentationProfiler(const char* label)
    : m_label(label)
{
    m_timer.Start();
}

SimpleInstrumentationProfiler::~SimpleInstrumentationProfiler()
{
    const double result = m_timer.GetTimer();

    // TODO: Change this to use our Logger!!!
    // Log the time.
    std::cout << "[" << m_label << "] Result: " << result << "ms\n";
}