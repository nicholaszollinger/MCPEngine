#pragma once
// ThreadIdleEvent.h

#include <atomic>
#include <condition_variable>
#include <mutex>

// #include Logger

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : This class defines the interface to manage when a Thread is idle/sleeping/waiting or working. The function
///         WaitUntilIdle() will block the process that called it until the associated Thread is put to sleep.
//-----------------------------------------------------------------------------------------------------------------------------
class ThreadIdleEvent
{
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_isIdle;

public:
    ThreadIdleEvent(const bool startIdle = true) : m_isIdle(startIdle) { }

    void Resume();
    void Signal();
    void WaitUntilIdle();
};