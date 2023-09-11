#pragma once
// Timer.h
 
class Timer
{
public:
    Timer();

    void Start();
    void Start(const long long duration);
    long long GetElapsedTime();

private:
    long long m_elapsedTime;
    long long m_startTime;
};