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

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Get a copy of the internal string. If you just want a reference, use GetStringRef(). \n NOT THREAD SAFE.
//-----------------------------------------------------------------------------------------------------------------------------
std::string StringId::GetStringCopy() const
{
    return *m_pStrRef;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Get a const reference to the internal string. If you want a copy, use GetStringCopy(). \n NOT THREAD SAFE.
//-----------------------------------------------------------------------------------------------------------------------------
const std::string& StringId::GetStringRef() const
{
    return *m_pStrRef;
}

const std::string& StringId::operator*() const
{
    return *m_pStrRef;
}

const std::string* StringId::GetConstPtr() const
{
    return m_pStrRef;
}

const char* StringId::GetCStr() const
{
    return m_pStrRef->c_str();
}


std::string* StringId::GetStringPtr(const char* str)
{
    // If we are being set to nullptr, then return the address of the invalid string.
    if (!str)
        return &s_invalidStringId;

    const uint32_t hash = HashString32(str);

    std::lock_guard lock(s_stringMutex);

    // If we don't have the string already, add it to our container.
    if (s_strings.find(hash) == s_strings.end())
    {
        s_strings[hash] = str;
    }

    return &s_strings[hash];
}

std::string* StringId::GetStringPtr(const std::string& str)
{
    if (str.empty())
        return &s_invalidStringId;

    return GetStringPtr(str.c_str());
}

uint64_t StringIdHasher::operator()(const StringId id) const
{
    static std::hash<const std::string*> hash;

    return hash(id.GetConstPtr());
}
