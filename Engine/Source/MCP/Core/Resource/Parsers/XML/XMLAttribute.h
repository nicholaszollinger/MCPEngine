#pragma once
// XMLAttribute.h

#include "MCP/Debug/Assert.h"
#include "MCP/Debug/Log.h"

namespace mcp
{
    class XMLAttribute
    {
        const void* m_pHandle = nullptr;

    public:
        XMLAttribute(const void* pHandle);

        XMLAttribute GetNext() const;

        template<typename AttributeType> AttributeType GetValue() const;
        [[nodiscard]] const char* GetName() const;
        [[nodiscard]] bool IsValid() const;

    private:
        [[nodiscard]] int64_t GetIntValue() const;
        [[nodiscard]] uint64_t GetUnsignedIntValue() const;
        [[nodiscard]] double GetDoubleValue() const;
        [[nodiscard]] bool GetBoolValue() const;
        [[nodiscard]] const char* GetStringValue() const;
    };

#pragma warning (push)
#pragma warning (disable : 4702)
    template <typename AttributeType>
    AttributeType XMLAttribute::GetValue() const
    {
        static_assert(std::is_arithmetic_v<AttributeType> 
            || std::is_same_v<AttributeType, bool> 
            || std::is_same_v<AttributeType, const char*>
            , "Attribute type must be a number type, bool or const char*");

        if constexpr (std::is_same_v<AttributeType, bool>)
        {
            return GetBoolValue();
        }

        else if constexpr (std::is_same_v<AttributeType, const char*>)
        {
            return GetStringValue();
        }

        else if constexpr (std::is_integral_v<AttributeType>)
        {
            if constexpr (std::is_unsigned_v<AttributeType>)
            {
                return static_cast<AttributeType>(GetIntValue());
            }

            else
            {
                return static_cast<AttributeType>(GetUnsignedIntValue());
            }
        }

        else if constexpr (std::is_floating_point_v<AttributeType>)
        {
            return static_cast<AttributeType>(GetDoubleValue());
        }

        // Shouldn't be able to get here
        return {};
    }
#pragma warning (pop)

}