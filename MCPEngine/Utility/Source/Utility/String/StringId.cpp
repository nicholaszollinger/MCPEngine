// StringId.cpp

#include "StringId.h"
#include "Generic/Hash.h"

StringId::StringId(const char* str)
    : m_pStrRef(GetStringPtr(str))
{
    //
}

StringId::StringId(const std::string& str)
    : m_pStrRef(GetStringPtr(str))
{
    //    
}

StringId::StringId(StringId&& right) noexcept
    : m_pStrRef(right.m_pStrRef)
{
    right.m_pStrRef = &s_invalidStringId;
}

StringId& StringId::operator=(StringId&& right) noexcept
{
    if (right != *this)
    {
        m_pStrRef = right.m_pStrRef;
        right.m_pStrRef = &s_invalidStringId;
    }

    return *this;
}

std::string StringId::GetStringCopy() const
{
    return *m_pStrRef;
}

const std::string& StringId::GetString() const
{
    return *m_pStrRef;
}

const std::string& StringId::operator*() const
{
    return *m_pStrRef;
}

std::string* StringId::GetStringPtr(const char* str)
{
    const uint32_t hash = HashString32(str);

    // If we don't have the string already, add it to our container.
    if (s_strings.find(hash) != s_strings.end())
    {
        s_strings[hash] = str;
    }

    return &s_strings[hash];
}

std::string* StringId::GetStringPtr(const std::string& str)
{
    return GetStringPtr(str.c_str());
}