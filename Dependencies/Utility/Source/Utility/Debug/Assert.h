#pragma once
// Assert.h
#include "../Logging/Log.h"

#undef _CHECK

#if _DEBUG
#define _CHECK(condition)                                                                                                           \
    do                                                                                                                              \
    {                                                                                                                               \
        if (!(condition))                                                                                                           \
        {                                                                                                                           \
            _ERROR("Assertion Failed!", (#condition), "\n\t", GET_CURRENT_FILENAME, " - ", __FUNCTION__, "() - Line: ", __LINE__);  \
            __debugbreak();                                                                                                         \
        }                                                                                                                           \
    } while(0)

#define _CHECK_MSG(condition, ...)                                                                                                  \
    do                                                                                                                              \
    {                                                                                                                               \
        if (!(condition))                                                                                                           \
        {                                                                                                                           \
            _ERROR("Assertion Failed!", __VA_ARGS__, "\n\t" , GET_CURRENT_FILENAME, " - ", __FUNCTION__, "() - Line: ", __LINE__);  \
            __debugbreak();                                                                                                         \
        }                                                                                                                           \
    } while(0)

#else
#define _CHECK(condition) void(0)
#define _CHECK_MSG(condition, ...) void(0)
#endif