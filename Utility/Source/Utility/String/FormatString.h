#pragma once
// CombineIntoString.h

#include <string>
#include "../Generic/ConceptTypes.h"

// Checks to see if we can perform 'std::ostream << Type;'
GENERATE_TYPE_CHECK(OutStreamableType, std::declval<std::ostream>() << std::declval<Type>());

// Checks to see if we can use 'std::to_string(Type);'
GENERATE_TYPE_CHECK(ToStringType, std::to_string(std::declval<Type>()));

// Checks to see if we can use the '+=' operator with this type -> std::string str += Type;
GENERATE_TYPE_CHECK(AddableToStringType, std::declval<std::string>() += std::declval<Type>());

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Calculate the size of a c-style string at compile time.
//-----------------------------------------------------------------------------------------------------------------------------
constexpr size_t StrLength(const char* str)
{
    const char* pCursor = str;
    while (*pCursor != '\0')
        ++pCursor;

    return pCursor - str;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Adds a value of type Arg to a string, if possible.
///		@tparam Arg : Type must either be able to be used in std::to_string, or is addable to a std::string. 
///		@param str : std::string to add to.
///		@param arg : Value to add to str.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename Arg>
void AddToString(std::string& str, const Arg& arg)
{
    static_assert(ToStringType<Arg>::value || AddableToStringType<Arg>::value
        , "Type must either be able to be used in std::to_string, or is addable to a std::string!");

    // We can use std::to_string(). NOTE: to_string() will convert a char to its numerical type.
    // So we pass that off to the next branch.
    if constexpr (ToStringType<Arg>::value && !std::is_same_v<char, Arg>)
    {
        str += std::to_string(arg);
    }

    else if constexpr (AddableToStringType<Arg>::value)
    {
        str += arg;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      TODO: I want to put it somewhere more general.
//      This is a fold expression that recursively calls each void function.
//
///		@brief : 
///		@tparam FuncTypes : 
///		@param func : 
//-----------------------------------------------------------------------------------------------------------------------------
template<typename...FuncTypes>
void InvokeAll(FuncTypes&&... func)
{
    (func(), ...);
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Concatenates each of the arg values into a single std::string.
///		@returns : The concatenated std::string.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename...Args>
std::string CombineIntoString(const Args&... args)
{
    std::string output{};

    // Source: https://en.cppreference.com/w/cpp/language/fold
    InvokeAll([&]()
        { AddToString(output, args); }...);

    return output;
}

namespace Internal
{
    void FormatStringImpl(const char* pFormat, std::string& outStr);

    template<typename FirstType, typename...Args>
    void FormatStringImpl(const char* pFormat, std::string& outStr, const FirstType& first, const Args&...args)
    {
        for (; *pFormat != '\0'; pFormat++)
        {
            if (*pFormat == '%')
            {
                ++pFormat;
                AddToString(outStr, first);
                FormatStringImpl(pFormat, outStr, args...);
                return;
            }

            outStr += *pFormat;
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      This based of the 'tprintf' function on cppreference: https://en.cppreference.com/w/cpp/language/parameter_pack
//
///		@brief : Create a std::string that follows the format.
///         \n FORMAT RULES:
///         \n Anytime you want to insert an argument, put a '%' character into the formatted string.
///         \n Each argument following the format string will be added to the final string in order.
///         \n\n EXAMPLE:
///         \n FormatString("% world% %", "Hello", '!', 123); will print: "Hello World! 123".
///         \n (You can add a newline char to the format string, I can't here because of comment formatting.)\n
///		@tparam Args : Any type'd arguments that you want to add into the std::string.
///		@param pFormat : String that defines the format of the final output.
///		@param args : Values to add into the string in sequential order.
///		@returns : Formatted string.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename...Args>
std::string FormatString(const char* pFormat, const Args&...args)
{
    std::string output;
    Internal::FormatStringImpl(pFormat, output, args...);
    return output;
}

std::string FormatCurrentTime(const char* pFormat);
std::string FormatTwoDigit(const int val);