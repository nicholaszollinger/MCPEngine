#pragma once
// WindowsConsoleWindow.h

#include <string>

namespace mcp
{
    namespace LoggerInternal
    {
        enum class LogType;
    }

    class WindowsConsoleWindow
    {
    public:
        WindowsConsoleWindow() = default;

        void Post(const char* pMsg);
        void Post(const std::string& msg);
        void SetColor(const LoggerInternal::LogType& color);
        void Close();
    };
}