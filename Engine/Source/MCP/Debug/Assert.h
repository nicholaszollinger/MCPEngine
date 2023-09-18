#pragma once

#include "Utility/Debug/Assert.h"

// TODO MAYBE: Unify the two into one MCP_CHECK by checking if __VA_ARGS__ contains anything?
//   #define HAS_ARGS(...) (""#__VA_ARGS__[0] != '\0')

#define MCP_CHECK(condition) _CHECK(condition)
#define MCP_CHECK_MSG(condition, ...) _CHECK_MSG(condition, __VA_ARGS__)