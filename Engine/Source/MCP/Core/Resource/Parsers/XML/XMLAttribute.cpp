// XMLAttribute.cpp

#include "XMLAttribute.h"

#if MCP_XML_PLATFORM == XML_PLATFORM_TINYXML2
#include "Platform/TinyXML2/tinyxml2.h"

#define CAST_TO_ATTRIBUTE_TYPE(ptr) static_cast<tinyxml2::XMLAttribute*>((ptr))
#define CAST_TO_CONST_ATTRIBUTE_TYPE(ptr) static_cast<const tinyxml2::XMLAttribute*>((ptr))

#else
#error "We don't have a XMLFile implementation for the current selected XML Platform!"
#endif

namespace mcp
{
    XMLAttribute::XMLAttribute(const void* pHandle)
        : m_pHandle(pHandle)
    {
        //
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the next attribute adjacent to this XMLAttribute. Note, this can result in an Invalid XMLAttribute. Make sure
    ///         to check before using the result!
    //-----------------------------------------------------------------------------------------------------------------------------
    XMLAttribute XMLAttribute::GetNext() const
    {
        MCP_CHECK(IsValid());

        auto* pNext =  CAST_TO_CONST_ATTRIBUTE_TYPE(m_pHandle)->Next();
        return XMLAttribute(pNext);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns whether this is a valid Attribute or not.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool XMLAttribute::IsValid() const
    {
        return m_pHandle;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the name of this attribute.
    //-----------------------------------------------------------------------------------------------------------------------------
    const char* XMLAttribute::GetName() const
    {
        MCP_CHECK(IsValid());
        return CAST_TO_CONST_ATTRIBUTE_TYPE(m_pHandle)->Name();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the value of the attribute as a boolean.
    ///		@returns : Boolean value regardless. In debug mode, it will report an error if there was a conversion issue. If
    ///         there is an issue, it will return false.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool XMLAttribute::GetBoolValue() const
    {
        const auto* pAttribute = CAST_TO_CONST_ATTRIBUTE_TYPE(m_pHandle);

#if _DEBUG
        bool result{};
        const tinyxml2::XMLError errorCode = pAttribute->QueryBoolValue(&result);

        if (errorCode == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE)
        {
            MCP_ERROR("XML", "Failed to get attribute value! Cannot convert xml value to bool! Attribute name:" , pAttribute->Name());
        }

        return result;
#else
        return pAttribute->BoolValue();
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the value of the attribute as an int64_t.
    ///		@returns : Int64_t value regardless. In debug mode, it will report an error if there was a conversion issue. If
    ///         there is an issue, it will return 0.
    //-----------------------------------------------------------------------------------------------------------------------------
    int64_t XMLAttribute::GetIntValue() const
    {
        const auto* pAttribute = CAST_TO_CONST_ATTRIBUTE_TYPE(m_pHandle);
#if _DEBUG
        int64_t result{};
        const tinyxml2::XMLError errorCode = pAttribute->QueryInt64Value(&result);

        if (errorCode == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE)
        {
            MCP_ERROR("XML", "Failed to get attribute value! Cannot convert xml value to int64_t! Attribute name:" , pAttribute->Name());
        }

        return result;
#else
        return pAttribute->Int64Value();
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the value of the attribute as an uint64_t.
    ///		@returns : Uint64_t value regardless. In debug mode, it will report an error if there was a conversion issue. If
    ///         there is an issue, it will return 0.
    //-----------------------------------------------------------------------------------------------------------------------------
    uint64_t XMLAttribute::GetUnsignedIntValue() const
    {
        const auto* pAttribute = CAST_TO_CONST_ATTRIBUTE_TYPE(m_pHandle);
#if _DEBUG
        uint64_t result{};
        const tinyxml2::XMLError errorCode = pAttribute->QueryUnsigned64Value(&result);

        if (errorCode == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE)
        {
            MCP_ERROR("XML", "Failed to get attribute value! Cannot convert xml value to unt64_t! Attribute name:" , pAttribute->Name());
        }

        return result;
#else
        return pAttribute->Unsigned64Value();
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the value of the attribute as a double.
    ///		@returns : Double value regardless. In debug mode, it will report an error if there was a conversion issue. If
    ///         there is an issue, it will return 0.
    //-----------------------------------------------------------------------------------------------------------------------------
    double XMLAttribute::GetDoubleValue() const
    {
        const auto* pAttribute = CAST_TO_CONST_ATTRIBUTE_TYPE(m_pHandle);
#if _DEBUG
        double result{};
        const tinyxml2::XMLError errorCode = pAttribute->QueryDoubleValue(&result);

        if (errorCode == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE)
        {
            MCP_ERROR("XML", "Failed to get attribute value! Cannot convert xml value to double! Attribute name:" , pAttribute->Name());
        }

        return result;
#else
        return pAttribute->DoubleValue();
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the value of the attribute as a string.
    //-----------------------------------------------------------------------------------------------------------------------------
    const char* XMLAttribute::GetStringValue() const
    {
        const auto* pAttribute = CAST_TO_CONST_ATTRIBUTE_TYPE(m_pHandle);
        return pAttribute->Value();
    }
}