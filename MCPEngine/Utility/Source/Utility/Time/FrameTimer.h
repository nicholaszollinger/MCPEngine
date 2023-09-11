#pragma once
// FrameTimer.h

class FrameTimer
{
public:
    void Init();
    float NewFrame();

private:
    long long m_lastFrameTime = 0;
};