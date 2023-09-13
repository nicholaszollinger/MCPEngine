// Logger.cpp

#include "Logger.h"

#include <functional>

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Initialize the logger. NOTE: If you set the logger to be multithreaded, know that this function is not thread-safe,
///          because this is intended to be called before happens in the program, on the main thread.
//-----------------------------------------------------------------------------------------------------------------------------
bool Logger::Init(const char* logOutDirectoryPath)
{
    // Open a new log file.
    // TODO:
    //  - Make the logger settings part of a log config file.
    // Create the name for the Log file. The format will be Log-[Date][Time]
    const auto newFileName = CombineIntoString(logOutDirectoryPath, FormatCurrentTime("[%M-%D-%Y] %h-%m-%s%L"),".txt");
    s_outFile.open(newFileName.c_str());

    if (!s_outFile.is_open())
    {
        return false;
    }

    s_outFile << FormatCurrentTime("%M/%D/%Y : %h:%m %L") << "\n\n";

    return s_target.Init();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Close the logger. NOTE: If you set the logger to be multithreaded, know that this function is not thread-safe.
///         This is intended to be called at the very end of the program's lifetime, on the main thread.
//-----------------------------------------------------------------------------------------------------------------------------
void Logger::Close()
{
    s_target.Close();
    s_outFile.close();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Load log categories from data.
//-----------------------------------------------------------------------------------------------------------------------------
bool Logger::LoadCategories(const char* filepath)
{
    std::ifstream inFile(filepath, std::ios::in);
    if (!inFile.is_open())
    {
        VitalLog("Logger", LogType::kError,"Failed to open Log Categories at filepath: ", filepath);
        return false;
    }

    std::string line;

    while (std::getline(inFile, line))
    {
        if (line[0] == '#' || line[0] == ';')
            continue;

        const auto isWhiteSpace = [](const char val) -> bool
        {
           return val == ' '
                || val == '\t'
                || val == '\n'
                || val == '\r'
                || val == '\v'
                || val == '\f';
        };

        // Remove all whitespace from the line.
        // I can't use the c++20 version -> std::erase_if(line, isWhiteSpace);
        // So I just grabbed the implementation from cppreference and put it in a lambda.
        // https://en.cppreference.com/w/cpp/string/basic_string/erase2
        auto eraseIf = [](std::string& str, const std::function<bool(const char)>& predicate) -> void
        {
            const auto it = std::remove_if(str.begin(), str.end(), predicate);
            //auto r = std::distance(it, str.end());
            str.erase(it, str.end());
        };

        eraseIf(line, isWhiteSpace);

        if (line.empty())
            continue;

        const auto endOfName = line.find_first_of('=');
        auto name = line.substr(0, endOfName);
        if (name.empty())
            continue;

        auto outputVal = line.substr(endOfName + 1, 1);
        const LogOutput outputLevel = static_cast<LogOutput>(std::stoi(outputVal));
        const uint32_t hashedString = HashString32(name.c_str());

        s_categories.emplace(hashedString, LogCategory(name.c_str(), outputLevel));
    }

    return true;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : This is the stage we actually present the log onto the LogTarget. This is thread-safe if LOGGER_IS_MULTITHREADED
///             is set to true.
//-----------------------------------------------------------------------------------------------------------------------------
void Logger::PostToLogTarget(const LogType type, const std::string& msg)
{
    #if LOGGER_IS_MULTITHREADED
    std::lock_guard lock(s_mutex);
#endif
    
    s_target.PrePost(type);
    s_target.Post(msg);
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Write the message to the output file defined in Logger::Init(). This is thread-safe if LOGGER_IS_MULTITHREADED
///             is set to true.
//-----------------------------------------------------------------------------------------------------------------------------
void Logger::WriteToFile(const std::string& msg)
{
#if LOGGER_IS_MULTITHREADED
     std::lock_guard lock(s_mutex);
#endif
     
    s_outFile << msg;
}