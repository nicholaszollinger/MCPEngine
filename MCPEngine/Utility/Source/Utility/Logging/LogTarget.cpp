// LogTarget.cpp

#include "LogTarget.h"
#include "LogConfig.h"

#ifdef USE_DEFAULT_LOG_TARGET
#include "LogTargetConsoleWindow.h"
    using LogTargetImpl = LogTargetConsoleWindowImpl;
#endif

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Initialize the LogTarget, generally opening up its window.
///		@returns : True if successful.
//-----------------------------------------------------------------------------------------------------------------------------
bool LogTarget::Init()
{
    return GetImpl<LogTargetImpl>().Init();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Close the LogTarget, generally closing its window.
//-----------------------------------------------------------------------------------------------------------------------------
void LogTarget::Close()
{
    GetImpl<LogTargetImpl>().Close();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Used to perform any formatting operations per the LogType (like a different color font) before posting the Log.
///		@param type : Type of Log we are posting.
//-----------------------------------------------------------------------------------------------------------------------------
void LogTarget::PrePost(const LogType type)
{
    GetImpl<LogTargetImpl>().PrePost(type);
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Post the log.
///		@param msg : Formatted message that we are posting.
//-----------------------------------------------------------------------------------------------------------------------------
void LogTarget::Post(const std::string& msg)
{
    GetImpl<LogTargetImpl>().Post(msg);
}