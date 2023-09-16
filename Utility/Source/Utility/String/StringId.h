#pragma once
// StringId.h

#include <string>
#include <unordered_map>

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      This is an interned string implementation. I did the simple thing and just had the string database be a static member
//      on the class. That means that the database could get HUGE, and I would prefer to have a 'scope' or something (like
//      GameObjectName StringIds) but this was the simple thing.
//
///		@brief : A StringId is a pointer to a std::string that lives in a static database. Comparing StringIds is trivial because
///         we are just comparing the pointers.
//-----------------------------------------------------------------------------------------------------------------------------
class StringId
{
    // Easiest
    inline static std::unordered_map<uint32_t, std::string> s_strings = std::unordered_map<uint32_t, std::string>(64);
    inline static std::string s_invalidStringId = "Invalid StringId";

    std::string* m_pStrRef = &s_invalidStringId;

public:
    StringId() = default;
    StringId(const char* str);
    StringId(const std::string& str);

    StringId(const StringId& right) = default;
    StringId(StringId&& right) noexcept;
    StringId& operator=(const StringId& right) = default;
    StringId& operator=(StringId&& right) noexcept;
    ~StringId() = default;

    std::string GetStringCopy() const;
    const std::string& GetString() const;
    const std::string& operator*() const;

    constexpr bool operator==(const StringId& right) const { return m_pStrRef == right.m_pStrRef; }
    constexpr bool operator!=(const StringId& right) const { return !(*this == right); }

private:
    static std::string* GetStringPtr(const char* str);
    static std::string* GetStringPtr(const std::string& str);
};