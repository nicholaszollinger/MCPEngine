#pragma once
// DefaultLogTarget.h

#include <string>
#include "LogType.h"

class LogTargetConsoleWindowImpl
{
public:
    bool Init();
    void Close();
    void PrePost(const LogType type);
    void Post(const std::string& msg);
};