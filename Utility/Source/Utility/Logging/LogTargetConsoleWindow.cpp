// LogTargetConsoleWindow.cpp

#include "LogTargetConsoleWindow.h"

#if _WIN32
#include <conio.h>
#include <iostream>
#pragma warning (push)
#pragma warning (disable : 5105)
#include <Windows.h>
#pragma warning (pop)

bool LogTargetConsoleWindowImpl::Init()
{
    AllocConsole();
    return true;
}

void LogTargetConsoleWindowImpl::Close()
{
    PrePost(LogType::kLog);

    // A pause before exiting the program.
    Post("Press any key to close...\n");
    _getch();
    FreeConsole();
}

void LogTargetConsoleWindowImpl::PrePost(const LogType type)
{
    const HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    switch (type)
    {
        case LogType::kLog:
            // Grey Color
            SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
            break;
        
        case LogType::kWarning:
            // Yellow Color
            SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            break;
    
        case LogType::kError:
        case LogType::kCritical:
            // Red Color
            SetConsoleTextAttribute(h, FOREGROUND_RED);
            break;
    }
}

void LogTargetConsoleWindowImpl::Post(const std::string& msg)
{
    std::cout << msg;
}

#else

bool LogTargetConsoleWindowImpl::Init()
{
    return false;
}

void LogTargetConsoleWindowImpl::Close() {}
void LogTargetConsoleWindowImpl::PrePost([[maybe_unused]] const LogType type) {}
void LogTargetConsoleWindowImpl::Post([[maybe_unused]] const std::string& msg) {}

#endif