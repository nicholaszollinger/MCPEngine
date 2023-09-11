#pragma once
// Config.h

// Goal: I want all of my global macros to be defined here.
//  This includes things like Platform (Windows), RenderLibrary (Ex : SDL2)
//  I want to have a MPC prefix on all of these macros so that I can easily identify the engine code

//--------------------------------------------------------------------------------------------------------------
//      OS PLATFORM DEFINITIONS
//--------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
    #define MCP_PLATFORM_WINDOWS
#else
    #error "MCP Engine does not support this platform!"
#endif

//--------------------------------------------------------------------------------------------------------------
//      WINDOW PLATFORM
//--------------------------------------------------------------------------------------------------------------
#define MCP_WINDOW_PLATFORM_SDL 0

#define MCP_WINDOW_PLATFORM MCP_WINDOW_PLATFORM_SDL

//--------------------------------------------------------------------------------------------------------------
//      RENDERER API
//--------------------------------------------------------------------------------------------------------------
// Supported Renderer API's.
#define MCP_RENDERER_API_SDL 0

// The Renderer API that we are going to be using.
#define MCP_RENDERER_API MCP_RENDERER_API_SDL

//--------------------------------------------------------------------------------------------------------------
//      INPUT PLATFORM
//--------------------------------------------------------------------------------------------------------------
#define MCP_INPUT_PLATFORM_SDL 0

#define MCP_INPUT_PLATFORM MCP_INPUT_PLATFORM_SDL

//--------------------------------------------------------------------------------------------------------------
//      AUDIO PLATFORM
//--------------------------------------------------------------------------------------------------------------
#define MCP_AUDIO_PLATFORM_SDL 0

#define MCP_AUDIO_PLATFORM MCP_AUDIO_PLATFORM_SDL

//--------------------------------------------------------------------------------------------------------------
//      DATA LOADING LIB - What library or system we are using to load XML files
//--------------------------------------------------------------------------------------------------------------
#define MCP_DATA_PARSER_TINYXML2