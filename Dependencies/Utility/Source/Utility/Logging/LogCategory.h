#pragma once
// LogCategory.h

#include <string>

enum class LogOutput : int
{
    kNone,
    kDisplay,
    kFile,
    kAll,
};

class LogCategory
{
    std::string m_name;
    LogOutput m_output = LogOutput::kNone;

public:
    LogCategory() = default;
    LogCategory(const char* pName, const LogOutput outputLevel);
    LogCategory(const LogCategory&) = delete;
    LogCategory(LogCategory&& right) noexcept = default;
    LogCategory& operator=(const LogCategory&) = delete;
    LogCategory& operator=(LogCategory&& right) noexcept = default;
    ~LogCategory() = default;
    
    const std::string& GetName() const { return m_name; }
    LogOutput GetOutputLevel() const { return m_output; }
};