#pragma once
#include "StringId.h"

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      Do I want this to be a StringId instead of a std::string? Probably?
//		
///		@brief : A string wrapped in to have path specific functionality.
//-----------------------------------------------------------------------------------------------------------------------------
class Path
{
private:
    std::string m_val;

public:
    Path() = default;
    Path(const char* pPath) : m_val(pPath) {}
    Path(const std::string& path) : m_val(path) {}
    explicit Path(std::string&& path) : m_val(std::move(path)) {}

    [[nodiscard]] bool IsValid() const { return !m_val.empty(); }
    [[nodiscard]] const std::string& GetString() const { return m_val; }
    [[nodiscard]] const char* GetCStr() const { return m_val.c_str(); }
    [[nodiscard]] bool ExtensionMatches(const char* extension) const;
    [[nodiscard]] std::string GetExtenstion() const { return m_val.substr(m_val.find_last_of('.', std::string::npos)); }
};