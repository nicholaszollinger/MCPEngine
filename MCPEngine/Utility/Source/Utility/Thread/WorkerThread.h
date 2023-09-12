#pragma once
// WorkerThread.h

#include "ThreadIdleEvent.h"
#include <queue>
#include <thread>
#include <functional>
// #include Logger

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Default implementation of the ThreadInstructionType for the WorkerThread. Contains instructions for initializing,
///         running and terminating the WorkerThread.
//-----------------------------------------------------------------------------------------------------------------------------
enum class DefaultThreadInstruction
{
    kInit,
    kRun,
    kTerminate,
};

template<typename InstructionType = DefaultThreadInstruction>
class WorkerThread
{
public:
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Function for handling Instructions sent to this thread. If the function returns false, the thread is terminated.
    ///             \n An instruction is an enum value. The instruction handler should basically contain a switch statement that calls
    ///             different functions based on the instruction.
    //-----------------------------------------------------------------------------------------------------------------------------
    using ThreadInstructionHandler = std::function<bool(InstructionType)>;

private:
    std::thread m_thread;

    ThreadIdleEvent m_idleEvent;                // Interface for handling when this thread is put to sleep or not.
    std::mutex m_instructionMutex;              // Mutex that will guard accesses to the instruction queue.
    std::condition_variable m_wakeCondition;    // Condition variable for waking up the thread.
    std::atomic_bool m_isTerminated;            // Whether the thread has been terminated or not.

    std::queue<InstructionType> m_instructionQueue; // Queue of instructions to be processed on the thread.
    ThreadInstructionHandler m_instructionHandler;  // Function given to the thread to process incoming instructions.

public:
    WorkerThread() = default;
    WorkerThread(const WorkerThread&) = delete;
    WorkerThread(WorkerThread&& right) noexcept = default;
    WorkerThread& operator=(const WorkerThread&) = delete;
    WorkerThread& operator=(WorkerThread&& right) noexcept = default;
    ~WorkerThread();

    void Start(ThreadInstructionHandler&& handlerFunc);
    void Terminate();

    void SendInstruction(const InstructionType instruction);
    void SendInstructionWithoutNotify(const InstructionType instruction);
    void NotifyOfInstruction();
    void WaitUntilDone();

private:
    void ProcessInstructions();
    void SignalIdle();
};

// IMPLEMENTATION BELOW

template<typename InstructionType>
WorkerThread<InstructionType>::~WorkerThread()
{
    Terminate();   
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Begins the threads execution. NOTE: This will fail if called on a non-terminated thread!
///		@param handlerFunc : Function to handle incoming instructions sent to this thread.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename InstructionType>
void WorkerThread<InstructionType>::Start(ThreadInstructionHandler&& handlerFunc)
{
    if (m_thread.joinable())
    {
        //_WARN("Thread", "Tried to initialize a WorkerThread that is already running!");
        return;
    }

    m_isTerminated = false;
    m_instructionHandler = std::forward<ThreadInstructionHandler>(handlerFunc);

    // TODO: Consider adding a way to track the thread name, so you can see it
    // in visual studio, or use it for debugging purposes.
    m_thread = std::thread([this]() -> void { return ProcessInstructions();});
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Tells the thread to shut down and join back up with the main thread.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename InstructionType>
void WorkerThread<InstructionType>::Terminate()
{
    if (m_thread.joinable())
    {
        //_LOG("Thread", "Terminating thread...");

        // Exit our Process Work function.
        m_isTerminated = true;

        // Clear out our instruction queue, we are terminating... Not necessary because we early out when m_isTerminated is true.
        /*m_instructionMutex.lock();
        m_threadInstructionQueue.c.clear();
        m_instructionMutex.unlock();*/

        // Signal to any waiting external threads that this thread is done.
        m_idleEvent.Signal();

        // Notify the worker therad.
        m_wakeCondition.notify_all();

        // Join with the main thread.
        m_thread.join();
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Send an instruction to this WorkerThread. The thread will be immediately notified of the new instruction.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename InstructionType>
void WorkerThread<InstructionType>::SendInstruction(const InstructionType instruction)
{
    SendInstructionWithoutNotify(instruction);
    NotifyOfInstruction();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Add instructions to the thread's internal queue without letting it know that it should wake up. This is useful if you
///         want to queue up a bunch of instructions and not have to notify the thread for every add. When you finished queueing up
///         instructions, call NotifyOfInstruction() to let the thread know it is time to wake up.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename InstructionType>
void WorkerThread<InstructionType>::SendInstructionWithoutNotify(const InstructionType instruction)
{
    m_instructionMutex.lock();

    // Add the instruction to our queue.
    m_instructionQueue.push(instruction);

    m_instructionMutex.unlock();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Let's the thread know that there are instructions to be processed. If the thread was idle, it will wake up and begin work
///             again. This function is to be used in tandem with AddWorkWithoutNotify(), so that you can queue up a bunch of
///             work before resuming execution.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename InstructionType>
void WorkerThread<InstructionType>::NotifyOfInstruction()
{
    std::lock_guard lock(m_instructionMutex);
    // If we were notified, but don't actually have any work, then return.
    if (m_instructionQueue.empty())
        return;

    // Notify the two condition variables that we are now working.
    m_idleEvent.Resume();
    m_wakeCondition.notify_all();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Blocking function that will wait until the thread has finished running through its instructions.
///         Good for syncing between threads.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename InstructionType>
void WorkerThread<InstructionType>::WaitUntilDone()
{
    m_idleEvent.WaitUntilIdle();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : This is the main loop of the thread's execution, processing any instructions it is given.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename InstructionType>
void WorkerThread<InstructionType>::ProcessInstructions()
{
    while (!m_isTerminated)
    {
        // Signal that this thread is sleeping/idle
        SignalIdle();

        std::unique_lock lock(m_instructionMutex);
        // If we are done or our instruction queue is not empty, then we need to wake up.
        m_wakeCondition.wait(lock, [this]() -> bool { return m_isTerminated || !m_instructionQueue.empty(); });

        // If we are done, let's exit.
        if (m_isTerminated)
            continue;

        m_idleEvent.Resume();

        // While we have instructions available,.
        while(!m_instructionQueue.empty())
        {
            InstructionType instruction = m_instructionQueue.front();
            m_instructionQueue.pop();
            lock.unlock();

            // Send the instruction to the instruction handler.
            // If the function returns false, then we stop processing, the thread's work has resulted in a termination.
            if (!m_instructionHandler(instruction))
            {
                m_isTerminated = true;
                break;
            }

            lock.lock();
        }
    }
}

template<typename InstructionType>
void WorkerThread<InstructionType>::SignalIdle()
{
    m_idleEvent.Signal();
}