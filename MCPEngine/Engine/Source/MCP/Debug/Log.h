#pragma once
// Log.h

//-----------------------------------------------------------------------------------------------------------------------------
//		SIMPLE OVERVIEW:
//      Each log macro takes in a "who" and then the "what". The "who" is the class or system responsible for the message.
//      The "what" is whatever you want to write in the message. Arguments are separated by a comma and each argument must be
//      'addable' to a string.
//
//          -EXAMPLE: MCP_LOG("Application", "The Application has started! Application name: ", applicationName);
//              - Result -> "[Time of the Log] [Application] The Application has started! Application name: Cool Game"
//
//      MCP_LOG     -> Simple logging messages.
//      MCP_WARN    -> Warnings, will always be posted.
//      MCP_ERROR   -> Errors, will always be posted.
//      MCP_CRITICAL-> Used for errors that completely break the program. Mainly engine level stuff.
//
//-----------------------------------------------------------------------------------------------------------------------------

#include <string>
#include "MCP/Core/Config.h"
#include "MCP/Core/GlobalManager.h"
#include "MCP/Core/Macros.h"
#include "Utility/String/FormatString.h"

// NEW LOGGING SYSTEM
#include "Utility/Logging/Log.h"

#ifdef _DEBUG
    #define MCP_LOGGING_ENABLED 1
#else 
    #define MCP_LOGGING_ENABLED 0
#endif

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      Pretty up the File Name for Debugging Messages:
//      Adapted from: https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
//           - strrchr() gets the last occurrence of a certain character in a const char*.
//           - It either returns a pointer to the spot where it finishes, or it returns nullptr if nothing is found.
//           - So, we either return the pointer to the path just after the final '\' OR we return the file path.
//		
///		@brief : Get just the filename and extension from the __FILE__ macro.
//-----------------------------------------------------------------------------------------------------------------------------
#define GET_CURRENT_FILENAME (strrchr(__FILE__, '\\')? strrchr(__FILE__, '\\') + 1 : __FILE__)

// Enable Multithreaded logging if the engine is multithreaded.
#if MCP_LOGGING_ENABLED
#if MCP_MULTITHREADING_ENABLED
#pragma warning (push)
#pragma warning (disable : 4005) // Disable the macro redefinition warning.
#define LOGGER_IS_MULTITHREADED 1
#pragma warning (pop)
#endif

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Log a message, passing in any number of arguments to be combined into a single string.
///		@param Category : Name of the category you want this log to be a part of. The "Who" or "Where" this log is coming from.
///             \nNOTE: If you haven't defined the log in data, the log will only be displayed to the LogTarget. If you want
///             to turn off the log or have it just write to a file, etc, you need to set that in the config file.
//-----------------------------------------------------------------------------------------------------------------------------
#define MCP_LOG(Category, ...) _LOG(Category, __VA_ARGS__) //Logger::Log(Category, __VA_ARGS__)

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Log a warning message, passing in any number of arguments to be combined into a single string.
///		@param Category : Name of the category you want this log to be a part of. The "Who" or "Where" this log is coming from.
//-----------------------------------------------------------------------------------------------------------------------------
#define MCP_WARN(Category, ...) _WARN(Category, __VA_ARGS__) //Logger::VitalLog(Category, LogType::kWarning, __VA_ARGS__)

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Log an error message, passing in any number of arguments to be combined into a single string.
///		@param Category : Name of the category you want this log to be a part of. The "Who" or "Where" this log is coming from.
//-----------------------------------------------------------------------------------------------------------------------------
#define MCP_ERROR(Category, ...) _ERROR(Category, __VA_ARGS__)//Logger::Error(Category, __VA_ARGS__)

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Log a critical message, passing in any number of arguments to be combined into a single string. NOTE, after
///         posting the message, this will break execution!
///     @param Category : Name of the category you want this log to be a part of. The "Who" or "Where" this log is coming from.
//-----------------------------------------------------------------------------------------------------------------------------
#define MCP_CRITICAL(Category, ...) _CRITICAL(Category, __VA_ARGS__) //Logger::Critical(Category, __VA_ARGS__)

#else

#define MCP_LOG(Category, ...) void(0)
#define MCP_WARN(Category, ...) void(0)
#define MCP_ERROR(Category, ...) void(0)
#define MCP_CRITICAL(Category, ...) void(0)

#endif