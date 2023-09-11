// CombineIntoString.cpp

#include "FormatString.h"

#include <cassert>
#include "../Time/Time.h"

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : If the value is single digit, it will add a 0 to the front of the value.
///		@param val : Two digit value to evaluate.
///		@returns : std::string of the number.
//-----------------------------------------------------------------------------------------------------------------------------
std::string FormatTwoDigit(const int val)
{
    std::string output;
    if (val < 10)
    {
        output += '0';
    }

    return output += std::to_string(val);
}

const char* ToMonth(const int month)
{
    assert(month < 12 && month >= 0);

    static constexpr const char* kMonthArray[12]
    {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "July", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    return kMonthArray[month];
}

std::string ToStandardHour(const int hour)
{
    if (hour > 12)
    {
        return std::to_string(hour - 12);
    }

    if (hour == 0)
    {
        return "12";
    }

    return std::to_string(hour);
}

const char* GetAmPm(const int hour)
{
    if (hour <= 12)
    {
        return "AM";
    }

    return "PM";
}

std::string ConvertCharToTimeInfo(const char key, const tm& info)
{
    switch (key)
    {
        case 'h': return ToStandardHour(info.tm_hour);
        case 'm': return FormatTwoDigit(info.tm_min);
        case 's': return FormatTwoDigit(info.tm_sec);
        case 'Y': return std::to_string(info.tm_year - 100);
        case 'M': return ToMonth(info.tm_mon);
        case 'D': return FormatTwoDigit(info.tm_mday);
        case 'L': return GetAmPm(info.tm_hour);
        default: return "";
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Formats the current time into a single readable string.
///		@param pFormat : The format that you want the time to be in.
///             \n FORMAT RULES:
///             \n Each special character must be have a '%' char in front of it.
///             \n KEY:
///             \n h : Hour, expressed in standard format. Ex: 1PM would be '1'.
///             \n m : Minute.
///             \n s : Second.
///             \n Y : Year. Ex: 2023 would be '23'.
///             \n M : Month in three letter format. Ex: January would be 'Jan'.
///             \n D : Day. 
///             \n L : Time of day. Either 'AM' or 'PM'
//-----------------------------------------------------------------------------------------------------------------------------
std::string FormatCurrentTime(const char* pFormat)
{
    std::string output;
    const auto tmInfo = GetCurrentTimeInfo();

    // Move through the const char* by each character.
    for (; *pFormat != '\0'; pFormat++)
    {
        if (*pFormat == '%')
        {
            ++pFormat;
            output += ConvertCharToTimeInfo(*pFormat, tmInfo);
        }

        else
        {
            output += *pFormat;
        }
    }

    return output;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Our recursive end. Upon getting to this overload, our formatting is finished, and we can just add the
///             last bit of formatting into the outStr.
//-----------------------------------------------------------------------------------------------------------------------------
void Internal::FormatStringImpl(const char* pFormat, std::string& outStr)
{
    outStr += pFormat;
}