// WindowsConsoleWindow.cpp
#include "WindowsConsoleWindow.h"

#include <iostream>
#include <Windows.h>
#include "MCP/Debug/LogFontTypes.h"

namespace mcp
{
    void WindowsConsoleWindow::Close()
    {
        SetColor(LoggerInternal::LogType::kDefault);
    }

    void WindowsConsoleWindow::Post(const char* pMsg)
    {
        std::cout << pMsg;
    }
    
    void WindowsConsoleWindow::Post(const std::string& msg)
    {
        std::cout << msg;
    }

    void WindowsConsoleWindow::SetColor(const LoggerInternal::LogType& color)
    {
        const HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

        switch (color)
        {
            case LoggerInternal::LogType::kDefault:
            case LoggerInternal::LogType::kLog:
            // Grey Color
            SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
            break;

            //// White Color
            //SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            //break;

        case LoggerInternal::LogType::kError:
            // Red Color
            SetConsoleTextAttribute(h, FOREGROUND_RED);
            break;

        case LoggerInternal::LogType::kWarning:
            // Yellow Color
            SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            break;
        }
    }
}
