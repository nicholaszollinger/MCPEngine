// Log.cpp

#include "Log.h"

#include <BleachNew.h>
#include <cassert>
#include <direct.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include "Utility/String/FormatString.h"

// Define which implementation that we are going to be using for this program, based on Platform (maybe renderer).
#ifdef MCP_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsConsoleWindow.h"

namespace mcp
{
    using LoggerTarget = WindowsConsoleWindow;
}

#else
#error "We don't have a Logger set up for that platform!'"
#endif

namespace mcp
{
    namespace LoggerInternal
    {
        bool Logger::Init()
        {
            if (_mkdir(kDirectory) < 0 && errno == ENOENT)
            {
                LogError("Failed to create Log Directory!");
                return false;
            }

            std::string filePath = kDirectory;
            filePath += kLogFilePath;

            m_pOutFile = BLEACH_NEW(std::ofstream);
            auto* pOutFileCast = static_cast<std::ofstream*>(m_pOutFile);
            pOutFileCast->open(filePath);

            if (!pOutFileCast->is_open())
            {
                LogError("Failed to open file at path: ");
                return false;
            }

            // Output a date stamp to the log.
            const auto dateStr = FormatCurrentTime("[%M/%D/%Y]\n\n");
            *pOutFileCast << dateStr;

            return true;
        }

        void Logger::Log(const char* pMsg)
        {
            GetLoggerTarget<LoggerTarget>().SetColor(LogType::kLog);
            const auto formattedMsg = FormatLog(LogType::kLog, pMsg);
            Post(formattedMsg);
        }

        void Logger::Log(const std::string& msg)
        {
            Log(msg.c_str());
        }

        void Logger::Warn(const char* pMsg)
        {
            GetLoggerTarget<LoggerTarget>().SetColor(LogType::kWarning);
            const auto formattedMsg = FormatLog(LogType::kWarning, pMsg);
            Post(formattedMsg);
        }

        void Logger::Warn(const std::string& msg)
        {
            Warn(msg.c_str());
        }

        void Logger::Error(const char* pMsg)
        {
            GetLoggerTarget<LoggerTarget>().SetColor(LogType::kError);
            const auto formattedMsg = FormatLog(LogType::kError, pMsg);
            Post(formattedMsg);
        }

        void Logger::Error(const std::string& msg)
        {
            Error(msg.c_str());
        }

        void Logger::Close()
        {
            GetLoggerTarget<LoggerTarget>().SetColor(LogType::kDefault);
            GetLoggerTarget<LoggerTarget>().Close();

            auto* pOutFileCast = static_cast<std::ofstream*>(m_pOutFile);
            pOutFileCast->close();

            BLEACH_DELETE(pOutFileCast);
            m_pOutFile = nullptr;
        }

        std::string Logger::FormatLog(const LogType type, const char* msg)
        {
            std::string output;

            switch (type)
            {
            case LogType::kDefault:
            case LogType::kLog:
                output = CombineIntoString(FormatCurrentTime("[%h:%m:%s %L] "), msg, "\n");
                break;

            case LogType::kError:
                output = CombineIntoString(FormatCurrentTime("[%h:%m:%s %L] [ERROR] "), msg, "\n");
                break;

            case LogType::kWarning:
                output = CombineIntoString(FormatCurrentTime("[%h:%m:%s %L] [WARNING] "), msg, "\n");
                break;

            default: break;
            }

            return output;
        }

        void Logger::Post(const std::string& msg)
        {
#ifdef _DEBUG
            GetLoggerTarget<LoggerTarget>().Post(msg);
#endif

            // Put the Log into the output file.
            auto* pOutFileCast = static_cast<std::ofstream*>(m_pOutFile);
            *pOutFileCast << msg;
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Log a message to the target debug output.
    ///         \n Log messages can also be found in the output log file in the Log folder in the game project.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Log([[maybe_unused]] const char* pMsg)
    {
#ifdef _DEBUG
        LoggerInternal::Logger::Get()->Log(pMsg);
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //  
    ///		@brief : Log a warning message to the target debug output.
    ///         \n Log messages can also be found in the output log file in the Log folder in the game project.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LogWarning([[maybe_unused]] const char* pMsg)
    {
#ifdef _DEBUG
        LoggerInternal::Logger::Get()->Warn(pMsg);
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Log an error message to the target debug output.
    ///         \n Log messages can also be found in the output log file in the Log folder in the game project.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LogError([[maybe_unused]] const char* pMsg)
    {
#ifdef _DEBUG
        LoggerInternal::Logger::Get()->Error(pMsg);
#endif
    }
}