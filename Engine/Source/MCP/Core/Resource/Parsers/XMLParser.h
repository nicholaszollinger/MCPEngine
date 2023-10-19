#pragma once
// XMLParser.h

#include <type_traits>
#include "../Resource.h"
#include "XML/XMLAttribute.h"

namespace mcp
{
    // TODO: Move this to its own file.
    class XMLElement
    {
        void* m_pHandle = nullptr;

    public:
        XMLElement(void* pHandle);

        // Get connected Elements
        XMLElement GetChildElement(const char* pChildName = nullptr) const;
        XMLElement GetSiblingElement(const char* pSiblingName = nullptr) const;

        // Set Element Data
        template<typename AttributeType> void SetAttribute(const char* pAttributeName, const AttributeType val);

        // Get Element Data
        template<typename AttributeType> AttributeType GetAttributeValue(const char* pAttributeName, const AttributeType defaultVal = {}) const;
        XMLAttribute GetFirstAttribute() const;
        XMLAttribute GetAttribute(const char* pAttributeName) const;

        [[nodiscard]] const char* GetText() const;
        [[nodiscard]] const char* GetName() const;
        [[nodiscard]] bool IsValid() const;

    private:
        // Attributes
        int64_t GetIntAttribute(const char* pAttributeName, const int64_t defaultVal) const;
        uint64_t GetUnsignedIntAttribute(const char* pAttributeName, const uint64_t defaultVal) const;
        double GetDoubleAttribute(const char* pAttributeName, const double defaultVal) const;
        bool GetBoolAttribute(const char* pAttributeName, const bool defaultVal) const;
        const char* GetStringAttribute(const char* pAttributeName, const char* defaultVal) const;

        void SetBoolAttribute(const char* pAttributeName, const bool val) const;
        void SetStringAttribute(const char* pAttributeName, const char* val) const;
        void SetUnsignedIntAttribute(const char* pAttributeName, const uint64_t val) const;
        void SetDoubleAttribute(const char* pAttributeName, const double val) const;
        void SetIntAttribute(const char* pAttributeName, const int64_t val) const;
    };

#pragma warning (push)
#pragma warning (disable : 4702)
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set an attribute of the XMLElement.
    ///		@tparam AttributeType : 
    ///		@param pAttributeName : 
    ///		@param val : 
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename AttributeType>
    void XMLElement::SetAttribute(const char* pAttributeName, const AttributeType val)
    {
        static_assert(std::is_arithmetic_v<AttributeType> 
            || std::is_same_v<AttributeType, bool> 
            || std::is_same_v<AttributeType, const char*>
            , "Attribute type must be a number type, bool or const char*");

        MCP_CHECK(pAttributeName);
        MCP_CHECK_MSG(IsValid(), "Failed to get Attribute! XMLElement not connected to valid file!");

        if constexpr (std::is_same_v<AttributeType, bool>)
        {
            SetBoolAttribute(pAttributeName, val);
        }

        else if constexpr (std::is_same_v<AttributeType, const char*>)
        {
            SetStringAttribute(pAttributeName, val);
        }

        else if constexpr (std::is_integral_v<AttributeType>)
        {
            if constexpr (std::is_unsigned_v<AttributeType>)
            {
                SetIntAttribute(pAttributeName, val);
            }

            else
            {
                SetUnsignedIntAttribute(pAttributeName, val);
            }
        }

        else if constexpr (std::is_floating_point_v<AttributeType>)
        {
            SetDoubleAttribute(pAttributeName, val);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Attempt to get an Attribute value from this Element. If no attribute matching the name exists or if the conversion
    ///             of the value to AttributeType cannot be completed, this will return the default value provided. If an error occurs
    ///             a log will be posted explaining what happened.
    ///		@tparam AttributeType : Type of Attribute you want to get; Attribute type must be a number type, bool or const char*.
    ///		@param pAttributeName : Name of the Attribute you are looking for.
    ///		@param defaultVal : The default value that you want the return value to be in the event of an error. 
    ///		@returns : Value of the attribute, or the default value in the event of an error.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename AttributeType>
    AttributeType XMLElement::GetAttributeValue(const char* pAttributeName, const AttributeType defaultVal) const
    {
        static_assert(std::is_arithmetic_v<AttributeType> 
            || std::is_same_v<AttributeType, bool> 
            || std::is_same_v<AttributeType, const char*>
            , "Attribute type must be a number type, bool or const char*");

        MCP_CHECK(pAttributeName);
        MCP_CHECK_MSG(IsValid(), "Failed to get Attribute! XMLElement not connected to valid file!");

        if constexpr (std::is_same_v<AttributeType, bool>)
        {
            return GetBoolAttribute(pAttributeName, defaultVal);
        }

        else if constexpr (std::is_same_v<AttributeType, const char*>)
        {
            return GetStringAttribute(pAttributeName, defaultVal);
        }

        else if constexpr (std::is_integral_v<AttributeType>)
        {
            if constexpr (std::is_unsigned_v<AttributeType>)
            {
                return static_cast<AttributeType>(GetIntAttribute(pAttributeName, defaultVal));
            }

            else
            {
                return static_cast<AttributeType>(GetUnsignedIntAttribute(pAttributeName, defaultVal));
            }
        }

        else if constexpr (std::is_floating_point_v<AttributeType>)
        {
            return static_cast<AttributeType>(GetDoubleAttribute(pAttributeName, defaultVal));
        }

        // Shouldn't be able to get here
        return {};
    }

#pragma warning (pop)

    class XMLParser
    {
    private:
        class XMLFile final : public DiskResource
        {
        public:
            MCP_DEFINE_RESOURCE_DESTRUCTOR(XMLFile)
            virtual void Free() override;

        private:
            virtual void* LoadResourceType() override;
        };

        XMLFile m_loadedFile;

    public:
        XMLParser() = default;
        ~XMLParser();

        // Managing Files.
        [[nodiscard]] bool LoadFile(const char* pFilepath);
        [[nodiscard]] bool HasFileLoaded() const;
        void CloseCurrentFile();

        XMLElement GetElement(const char* pElementName = nullptr) const;
    };
}
