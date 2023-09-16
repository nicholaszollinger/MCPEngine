// ThreadIdleEvent.cpp

#include "ThreadIdleEvent.h"

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Sets the m_isIdle to false, denoting that the associated Thread is 'active'.
//-----------------------------------------------------------------------------------------------------------------------------
void ThreadIdleEvent::Resume()
{
    m_mutex.lock();
    m_isIdle = false;
    m_mutex.unlock();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Signal that the associated thread is now idle.
//-----------------------------------------------------------------------------------------------------------------------------
void ThreadIdleEvent::Signal()
{
    m_mutex.lock();
    m_isIdle = true;
    m_mutex.unlock();

    m_condition.notify_all();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Should be called from external threads. This function will block until ThreadIdleEvent.Signal() is called,
///         notifying that the associated thread is now idle.
//-----------------------------------------------------------------------------------------------------------------------------
void ThreadIdleEvent::WaitUntilIdle()
{
    // If we are already idle, then return.
    std::unique_lock lock(m_mutex);

    if (m_isIdle)
        return;
    
    m_condition.wait(lock, [this]() -> bool { return m_isIdle; });
}