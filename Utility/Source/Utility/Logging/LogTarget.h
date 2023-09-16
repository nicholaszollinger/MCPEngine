#pragma once
// LogTarget.h

#include <string>
#include "LogType.h"

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      I think I need to define a template target that is used. 'TargetImplementation'.
//      I am going to do this just because this is the analog to the C++20 version of things with modules,
//      and it is a method to get rid of virtual calls. Conditional Compilation
//		
///		@brief : A LogTarget is receiving end of Logs that will post them to the screen. By default, this is the console window.
//-----------------------------------------------------------------------------------------------------------------------------
class LogTarget
{
public:
    bool Init();
    void Close();
    void PrePost(const LogType type);
    void Post(const std::string& msg);

private:
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Get the platform defined WindowType.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename LogTargetImpl>
    LogTargetImpl& GetImpl()
    {
        static LogTargetImpl impl;
        return impl;
    }
};