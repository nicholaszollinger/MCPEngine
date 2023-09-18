#pragma once
// Log.h
#include "LogConfig.h"

#undef _LOG
#undef _WARN
#undef _ERROR
#undef _CRITICAL

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

#if LOGGING_ENABLED
#include "Logger.h"

#define _INIT_LOGGER(LogOutputFile) Logger::Init(LogOutputFile)
#define _CLOSE_LOGGER() Logger::Close()
#define _LOAD_LOG_CATEGORIES(CategoryDataFilepath) Logger::LoadCategories(CategoryDataFilepath)

#define _LOG(Category, ...) Logger::Log(Category, __VA_ARGS__)
#define _WARN(Category, ...) Logger::VitalLog(Category, LogType::kWarning, __VA_ARGS__)
#define _ERROR(Category, ...) Logger::VitalLog(Category, LogType::kError, __VA_ARGS__)
#define _CRITICAL(Category, ...) Logger::VitalLog(Category, LogType::kCritical, __VA_ARGS__)

#else

#define _INIT_LOGGER(LogOutputFile) void(0)
#define _CLOSE_LOGGER() void(0)
#define _LOG(Category, ...) void(0)
#define _WARN(Category, ...) void(0)
#define _ERROR(Category, ...) void(0)
#define _CRITICAL(Category, ...) void(0)

#define _LOAD_LOG_CATEGORIES(CategoryDataFilepath) void(0)

#endif