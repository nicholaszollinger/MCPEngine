#pragma once
// EnumHelpers

#include "Bit.h"

//-----------------------------------------------------------------------------------------------------------------------------
///		@brief : Create the bitwise operators for an Enum, to be able to use the enum as a bitmask.
//-----------------------------------------------------------------------------------------------------------------------------
#define DEFINE_ENUM_CLASS_BITWISE_OPERATORS(Enum)                                                                   \
    inline constexpr Enum operator|(const Enum left, const Enum right)                                              \
    {                                                                                                               \
        return static_cast<Enum>(                                                                                   \
            static_cast<std::underlying_type_t<Enum>>(left) |                                                       \
            static_cast<std::underlying_type_t<Enum>>(right));                                                      \
    }                                                                                                               \
                                                                                                                    \
    inline constexpr Enum operator&(const Enum left, const Enum right)                                              \
    {                                                                                                               \
        return static_cast<Enum>(                                                                                   \
            static_cast<std::underlying_type_t<Enum>>(left) &                                                       \
            static_cast<std::underlying_type_t<Enum>>(right));                                                      \
    }                                                                                                               \
                                                                                                                    \
    inline constexpr Enum operator^(const Enum left, const Enum right)                                              \
    {                                                                                                               \
        return static_cast<Enum>(                                                                                   \
            static_cast<std::underlying_type_t<Enum>>(left) ^                                                       \
            static_cast<std::underlying_type_t<Enum>>(right));                                                      \
    }                                                                                                               \
                                                                                                                    \
    inline constexpr Enum operator~(const Enum value)                                                               \
    {                                                                                                               \
        return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(value));                                \
    }                                                                                                               \
                                                                                                                    \
    inline constexpr Enum operator|=(Enum& left, const Enum right)                                                  \
    {                                                                                                               \
        return left = left | right;                                                                                 \
    }                                                                                                               \
                                                                                                                    \
    inline constexpr Enum operator&=(Enum& left, const Enum right)                                                  \
    {                                                                                                               \
        return left = left & right;                                                                                 \
    }                                                                                                               \
                                                                                                                    \
    inline constexpr Enum operator^=(Enum& left, const Enum right)                                                  \
    {                                                                                                               \
        return left = left ^ right;                                                                                 \
    }    