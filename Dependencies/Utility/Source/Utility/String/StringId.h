#pragma once
// StringId.h

#include <mutex>
#include <string>
#include <unordered_map>

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      This is an interned string implementation. The strings themselves are stored in a static container.
//
//      It would be cool if I could have a 'scope' to the StringIds. Instead of one unifying static container, it could have a
//      pointer to a container. The only reason this would be nice is for dealing with Threads. I could setup a StringId scope
//      to work on a single thread, for instance. Then we wouldn't have to lock. I am pretty sure I am going to have to make
//      this thread safe.
//
///		@brief : A StringId is a pointer to a std::string that lives in a static container of this class. Construction is the only
///         intensive process of this class, because we have to hash the string and find/add it to the container. So construct as
///         little as possible. Comparing, Copying, Moving, are all trivial. It's just a pointer. 
//-----------------------------------------------------------------------------------------------------------------------------
class StringId
{
    // Easiest
    inline static std::unordered_map<uint32_t, std::string> s_strings = std::unordered_map<uint32_t, std::string>(64);
    inline static std::string s_invalidStringId = "Invalid StringId";
    inline static std::mutex s_stringMutex;

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

    [[nodiscard]] std::string GetStringCopy() const;
    [[nodiscard]] const std::string& GetStringRef() const;
    [[nodiscard]] const std::string* GetConstPtr() const;
    [[nodiscard]] const char* GetCStr() const;
    [[nodiscard]] bool IsValid() const { return m_pStrRef != &s_invalidStringId; }

    const std::string& operator*() const;
    constexpr bool operator==(const StringId& right) const { return m_pStrRef == right.m_pStrRef; }
    constexpr bool operator!=(const StringId& right) const { return !(*this == right); }

private:
    static std::string* GetStringPtr(const char* str);
    static std::string* GetStringPtr(const std::string& str);
};

struct StringIdHasher
{
    uint64_t operator()(const StringId id) const;
};