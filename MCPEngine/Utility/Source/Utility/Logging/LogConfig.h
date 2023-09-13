#pragma once
// LogConfig.h

#ifdef _DEBUG
    #define LOGGING_ENABLED 1
#else
    #define LOGGING_ENABLED 0
#endif

// Undef this if you define another LogTarget implementation.
// The default log target is the Windows console window.
#define USE_DEFAULT_LOG_TARGET

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Toggle for enabling or disabling multithreaded logging. Logger is single threaded by default. Set this to 1
///         if you want to add mutex guards when using the Logger.
//-----------------------------------------------------------------------------------------------------------------------------
#define LOGGER_IS_MULTITHREADED 0