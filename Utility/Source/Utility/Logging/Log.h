#pragma once
// Log.h
#include "LogConfig.h"

#undef _LOG
#undef _WARN
#undef _ERROR
#undef _CRITICAL

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