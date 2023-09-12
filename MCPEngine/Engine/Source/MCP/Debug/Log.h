#pragma once
// Log.h

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES/PLANS:
//  
//      Needs:
//       - KISS!!!
//       - I need to decide whether I want to still create a log file in release mode, or if I just turn off
//         logging entirely... I was thinking about saving some sort of runtime history in release for bug reports
//         or something. Perhaps the logging of the game's state on a bug is in a different system...
//
//      Resources:
//       - I am looking at spdlog's github to get ideas on the formatting and basic capabilities that
//           I would like to support.
//       - Formatting like printf: https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
//       - TODO: Update to Roody's Logging system if you can.
//
//-----------------------------------------------------------------------------------------------------------------------------

// TODO: I want to wrap my includes in Macros.
//      This is so that I reduce the weight of the Logging system in release mode to 'zero'.
#include <string>
#include "LogFontTypes.h"
#include "MCP/Core/Config.h"
#include "MCP/Core/GlobalManager.h"
#include "MCP/Core/Macros.h"
#include "utility/Format/FormatString.h"

namespace mcp
{
    namespace LoggerInternal
    {
        class Logger final : public IProcess
        {
            static constexpr const char* kDirectory = "Log";            // This folder will be located in the Game's ProjectDir.
            static constexpr const char* kLogFilePath = "/MCPLog.txt";  // Name of the Default log file.
            static inline void* m_pOutFile = nullptr;                   // Pointer to the std::ofstream construct.

            DEFINE_GLOBAL_MANAGER(Logger)

        public:
            Logger() = default;

            virtual bool Init() override;
            virtual void Close() override;

            void Log(const char* pMsg);
            void Log(const std::string& msg);

            template<typename...Args>
            void Log(const char* pFormat, const Args&...args)
            {
                const std::string output(FormatString(pFormat, args...));
                Log(output.c_str());
            }

            void Warn(const char* pMsg);
            void Warn(const std::string& msg);

            template<typename...Args>
            void Warn(const char* pFormat, const Args&...args)
            {
                const std::string output(FormatString(pFormat, args...));
                Warn(output.c_str());
            }

            void Error(const char* pMsg);
            void Error(const std::string& msg);

            template<typename...Args>
            void Error(const char* pFormat, const Args&...args)
            {
                const std::string output(FormatString(pFormat, args...));
                Error(output.c_str());
            }

        private:
            std::string FormatLog(const LogType type, const char* msg);
            void Post(const std::string& msg);

            template<typename LoggerTarget>
            LoggerTarget& GetLoggerTarget()
            {
                static LoggerTarget target;
                return target;
            }
        };
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
//  GLOBAL INTERFACE BELOW
//-----------------------------------------------------------------------------------------------------------------------------
namespace mcp
{
    void Log(const char* pMsg);
    void LogWarning(const char* pMsg);
    void LogError(const char* pMsg);

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Log a message that follows the format rules below.
    ///         \n Log messages can also be found in the output log file in the Log folder in the game project.
    /// 
    ///         \n\n FORMAT RULES:
    ///         \n Anytime you want to insert an argument, put a '%' character into the formatted string.
    ///         \n Each argument following the format string will be added to the final string in order.
    ///         \n\n EXAMPLE:
    ///         \n FormatString("% world% %", "Hello", '!', 123); will produce the message: "Hello World! 123".
    ///         \n (You can add a newline char to the format string, I can't here because of comment formatting.)\n
    /// 
    ///		@tparam Args : Any type'd arguments that you want to add into the std::string.
    ///		@param pFormat : String that defines the format of the final output.
    ///		@param args : Values to add into the string in sequential order.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename...Args>
    void Log([[maybe_unused]] const char* pFormat, [[maybe_unused]] const Args&...args)
    {
#ifdef _DEBUG
        LoggerInternal::Logger::Get()->Log(pFormat, args...);
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Log a warning message that follows the format rules below.
    ///         \n Log messages can also be found in the output log file in the Log folder in the game project.
    /// 
    ///         \n\n FORMAT RULES:
    ///         \n Anytime you want to insert an argument, put a '%' character into the formatted string.
    ///         \n Each argument following the format string will be added to the final string in order.
    ///         \n\n EXAMPLE:
    ///         \n FormatString("% world% %", "Hello", '!', 123); will produce the message: "Hello World! 123".
    ///         \n (You can add a newline char to the format string, I can't here because of comment formatting.)\n
    /// 
    ///		@tparam Args : Any type'd arguments that you want to add into the std::string.
    ///		@param pFormat : String that defines the format of the final output.
    ///		@param args : Values to add into the string in sequential order.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename...Args>
    void LogWarning([[maybe_unused]] const char* pFormat, [[maybe_unused]] const Args&...args)
    {
#ifdef _DEBUG
        LoggerInternal::Logger::Get()->Warn(pFormat, args...);
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Log an error that follows the format rules below.
    ///         \n Log messages can also be found in the output log file in the Log folder in the game project.
    /// 
    ///         \n\n FORMAT RULES:
    ///         \n Anytime you want to insert an argument, put a '%' character into the formatted string.
    ///         \n Each argument following the format string will be added to the final string in order.
    ///         \n\n EXAMPLE:
    ///         \n FormatString("% world% %", "Hello", '!', 123); will produce the message: "Hello World! 123".
    ///         \n (You can add a newline char to the format string, I can't here because of comment formatting.)\n
    /// 
    ///		@tparam Args : Any type'd arguments that you want to add into the std::string.
    ///		@param pFormat : String that defines the format of the final output.
    ///		@param args : Values to add into the string in sequential order.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename...Args>
    void LogError([[maybe_unused]] const char* pFormat, [[maybe_unused]] const Args&...args)
    {
#ifdef _DEBUG
        LoggerInternal::Logger::Get()->Error(pFormat, args...);
#endif
    }
}