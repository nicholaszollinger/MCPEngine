#pragma once
// Logger.h

#include <fstream>
#include <mutex>
#include <unordered_map>
#include "LogCategory.h"
#include "LogConfig.h"
#include "LogTarget.h"
#include "../Generic/Hash.h"
#include "../String/FormatString.h"

class Logger
{
    static inline LogTarget s_target{};
    static inline std::unordered_map<uint32_t, LogCategory> s_categories{}; // Change this or are we good?
    static inline std::ofstream s_outFile{};
    static inline std::mutex s_mutex;

public:
    static bool Init(const char* logOutDirectoryPath);
    static void Close();

    static bool LoadCategories(const char* filepath);

    template<typename...AddableToStringTypes>
    static void Log(const char* pCategoryName, AddableToStringTypes&&...args);

    template<typename...AddableToStringTypes>
    static void VitalLog(const char* pCategoryName, const LogType type, AddableToStringTypes&&...args);

private:
    static void PostToLogTarget(const LogType type, const std::string& msg);
    static void WriteToFile(const std::string& msg);

    template<typename...AddableToStringTypes>
    static std::string FormatLog(const LogType type, const char* pCategoryName, AddableToStringTypes&&...args);
};

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Log a message to the log target. If the LogCategory isn't on, then this will NOT post the message.
///              HOWEVER, if this encounters a LogCategory that doesn't exist, then it will create the category and post
///              the log.
///		@tparam AddableToStringTypes : Arguments that can be safely added to a string. See 'FormatString.h -> AddToString()' for details.
///		@param pCategoryName : Name of the category that this log is associated with.
///		@param args : Arguments to pass into the log.
//-----------------------------------------------------------------------------------------------------------------------------
template <typename... AddableToStringTypes>
void Logger::Log(const char* pCategoryName, AddableToStringTypes&&... args)
{
    const uint32_t hashedString = HashString32(pCategoryName);

#if LOGGER_IS_MULTITHREADED
    s_mutex.lock();
#endif
    // If we don't have the category, we are going to make one that is display only.
    if (s_categories.find(hashedString) == s_categories.end())
    {
        s_categories.emplace(hashedString, LogCategory(pCategoryName, LogOutput::kDisplay));
    }

    // Get the associated category
    const auto& category = s_categories[hashedString];
#if LOGGER_IS_MULTITHREADED
    s_mutex.unlock();
#endif

    // Get the output level.
    const auto output = category.GetOutputLevel();
    // If the output level is none, we are skipping the log.
    if (output == LogOutput::kNone)
        return;

    // Format the log message:
    const auto msg = FormatLog(LogType::kLog, pCategoryName, args...);

    // Post to the target if we are going to display the log.
    if (output == LogOutput::kDisplay || output == LogOutput::kAll)
        PostToLogTarget(LogType::kLog, msg);

    // Write to the file if the output level allows it.
    if (output == LogOutput::kFile || output == LogOutput::kAll)
        WriteToFile(msg);
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Post a 'vital' log to the LogTarget. A 'vital' log are any warnings, errors, or critical messages.
///		@tparam AddableToStringTypes : Arguments that can be safely added to a string. See 'FormatString.h -> AddToString()' for details.
///		@param pCategoryName : Name of the category that this log is associated with.
///		@param type : Type of vital log that we are posting.
///		@param args : Arguments to pass into the log.
//-----------------------------------------------------------------------------------------------------------------------------
template <typename... AddableToStringTypes>
void Logger::VitalLog(const char* pCategoryName, const LogType type, AddableToStringTypes&&... args)
{
    const std::string msg = FormatLog(type, pCategoryName, args...);
    PostToLogTarget(type, msg);
    WriteToFile(msg);
}

template <typename... AddableToStringTypes>
std::string Logger::FormatLog(const LogType type, const char* pCategoryName, AddableToStringTypes&&... args)
{
    switch (type)
    {
        case LogType::kLog:
        {
            // Format: [CurrentTime] - [CategoryName] Message
            return CombineIntoString(FormatCurrentTime("[%h:%m:%s %L]"), " - [", pCategoryName, "] ", args..., "\n");
        }

        case LogType::kWarning:
        {
            // Format: [CurrentTime] - [WARNING:CategoryName] Message
            return CombineIntoString(FormatCurrentTime("[%h:%m:%s %L]"), " - [WARNING:", pCategoryName, "] ", args..., "\n");
        }

        case LogType::kError:
        {
            // Format: [CurrentTime] - [ERROR:CategoryName] Message
            return CombineIntoString(FormatCurrentTime("[%h:%m:%s %L]"), " - [ERROR:", pCategoryName, "] ", args..., "\n");
        }

        case LogType::kCritical:
        {
            // Format: [CurrentTime] - [CRITICAL:CategoryName] Message
            return CombineIntoString(FormatCurrentTime("[%h:%m:%s %L]"), " - [CRITICAL:", pCategoryName, "] ", args..., "\n");
        }
    }

    return "INVALID LOG TYPE!";
}
