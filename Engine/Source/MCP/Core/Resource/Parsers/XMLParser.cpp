// XMLParser.cpp

#include "XMLParser.h"

#include "MCP/Core/Resource/ResourceManager.h"

#if MCP_XML_PLATFORM == XML_PLATFORM_TINYXML2
#include "Platform/TinyXML2/tinyxml2.h"

#else
#error "We don't have a XMLFile implementation for the current selected XML Platform!"
#endif

// Helpful static cast shortcuts:
#define CAST_TO_FILE_TYPE(ptr) static_cast<tinyxml2::XMLDocument*>((ptr))
#define CAST_TO_CONST_ELEMENT_TYPE(ptr) static_cast<const tinyxml2::XMLElement*>((ptr))
#define CAST_TO_ELEMENT_TYPE(ptr) static_cast<tinyxml2::XMLElement*>((ptr))

//#define CAST_TO_PARSER_PTR(ptr) static_cast<mcp::XMLParser*>((ptr))

namespace mcp
{
    XMLElement::XMLElement(void* pHandle)
        : m_pHandle(pHandle)
    {
        //
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the first child of this Element that matches pChildName. If no name is given, then this returns the
    ///         first child element. This returns an invalid XMLElement if this Element had no children, or there were no children
    ///         that matched the given name.
    ///		@param pChildName : Name of the element you are looking for.
    //-----------------------------------------------------------------------------------------------------------------------------
    XMLElement XMLElement::GetChildElement(const char* pChildName) const
    {
        MCP_CHECK_MSG(IsValid(), "Failed to get Child Element! XMLElement not connected to valid file!");

        auto* pChild = CAST_TO_ELEMENT_TYPE(m_pHandle)->FirstChildElement(pChildName);
        return XMLElement(pChild);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the first child of this Element that matches pSiblingName. If no name is given, then this returns the
    ///         first sibling element. This returns an invalid XMLElement if this Element had no siblings, or there were no siblings
    ///         that matched the given name.
    ///		@param pSiblingName : Name of the sibling you are looking for.
    //-----------------------------------------------------------------------------------------------------------------------------
    XMLElement XMLElement::GetSiblingElement(const char* pSiblingName) const
    {
        MCP_CHECK_MSG(IsValid(), "Failed to get Sibling Element! XMLElement not connected to valid file!");

        auto* pSibling = CAST_TO_ELEMENT_TYPE(m_pHandle)->NextSiblingElement(pSiblingName);
        return XMLElement(pSibling);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the name of this XMLElement.
    //-----------------------------------------------------------------------------------------------------------------------------
    const char* XMLElement::GetName() const
    {
        MCP_CHECK_MSG(IsValid(), "Failed to get XMLElement name! XMLElement not connected to valid file!");
        return CAST_TO_CONST_ELEMENT_TYPE(m_pHandle)->Name();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns false if this element does not exist, or if the owning file is no longer in memory.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool XMLElement::IsValid() const
    {
        return m_pHandle && CAST_TO_CONST_ELEMENT_TYPE(m_pHandle)->GetDocument();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the character string of the text node, or nullptr if nothing is present.
    //-----------------------------------------------------------------------------------------------------------------------------
    const char* XMLElement::GetText() const
    {
        MCP_CHECK_MSG(IsValid(), "Failed to get XMLElement name! XMLElement not connected to valid file!");

        return CAST_TO_CONST_ELEMENT_TYPE(m_pHandle)->GetText();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the first attribute of this Element. Use this function if you don't know the type of element beforehand.
    ///		@returns : XMLAttribute class, which could be valid or not.
    //-----------------------------------------------------------------------------------------------------------------------------
    XMLAttribute XMLElement::GetFirstAttribute() const
    {
        MCP_CHECK_MSG(IsValid(), "Failed to get XMLElement name! XMLElement not connected to valid file!");
        return XMLAttribute(CAST_TO_CONST_ELEMENT_TYPE(m_pHandle)->FirstAttribute());
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Find an XMLAttribute that matches the name 'pAttributeName'. Note: this can result in an invalid XMLAttribute,
    ///         so be sure to check if it IsValid() before using the result!
    //-----------------------------------------------------------------------------------------------------------------------------
    XMLAttribute XMLElement::GetAttribute(const char* pAttributeName) const
    {
        MCP_CHECK_MSG(IsValid(), "Failed to get XMLElement name! XMLElement not connected to valid file!");
        return XMLAttribute(CAST_TO_CONST_ELEMENT_TYPE(m_pHandle)->FindAttribute(pAttributeName));
    }
    
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Try to get a boolean attribute named pAttributeName from the element. If the attribute is not found, it will return
    ///             the default value with NO message. If the attribute cannot be converted to the type, it will post an error.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool XMLElement::GetBoolAttribute(const char* pAttributeName, const bool defaultVal) const
    {
        const auto* pElement = CAST_TO_CONST_ELEMENT_TYPE(m_pHandle);

        bool result = defaultVal;
        const tinyxml2::XMLError errorCode = pElement->QueryBoolAttribute(pAttributeName, &result);

        if (errorCode == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE)
        {
            MCP_ERROR("XML", "Failed to get attribute value! Cannot convert xml value to bool! Attribute name: '", pAttributeName, "' | Element: ", pElement->Name());
        }

        return result;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Try to get a int64_t attribute named pAttributeName from the element. If the attribute is not found, it will return
    ///             the default value with NO message. If the attribute cannot be converted to the type, it will post an error.
    //-----------------------------------------------------------------------------------------------------------------------------
    int64_t XMLElement::GetIntAttribute(const char* pAttributeName, const int64_t defaultVal) const
    {
        const auto* pElement = CAST_TO_CONST_ELEMENT_TYPE(m_pHandle);
        int64_t result = defaultVal;
        const tinyxml2::XMLError errorCode = pElement->QueryInt64Attribute(pAttributeName, &result);

        if (errorCode == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE)
        {
            MCP_ERROR("XML", "Failed to get attribute value! Cannot convert xml value to signed integer! Attribute name: '", pAttributeName, "' | Element: ", pElement->Name());
        }

        return result;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Try to get a uint64_t attribute named pAttributeName from the element. If the attribute is not found, it will return
    ///             the default value with NO message. If the attribute cannot be converted to the type, it will post an error.
    //-----------------------------------------------------------------------------------------------------------------------------
    uint64_t XMLElement::GetUnsignedIntAttribute(const char* pAttributeName, const uint64_t defaultVal) const
    {
        const auto* pElement = CAST_TO_CONST_ELEMENT_TYPE(m_pHandle);
        uint64_t result = defaultVal;
        const tinyxml2::XMLError errorCode = pElement->QueryUnsigned64Attribute(pAttributeName, &result);

        if (errorCode == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE)
        {
            MCP_ERROR("XML", "Failed to get attribute value! Cannot convert xml value to unsigned integer! Attribute name: '", pAttributeName, "' | Element: ", pElement->Name());
        }

        return result;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Try to get a double attribute named pAttributeName from the element. If the attribute is not found, it will return
    ///             the default value with NO message. If the attribute cannot be converted to the type, it will post an error.
    //-----------------------------------------------------------------------------------------------------------------------------
    double XMLElement::GetDoubleAttribute(const char* pAttributeName, const double defaultVal) const
    {
        const auto* pElement = CAST_TO_CONST_ELEMENT_TYPE(m_pHandle);
        double result = defaultVal;
        const tinyxml2::XMLError errorCode = pElement->QueryDoubleAttribute(pAttributeName, &result);

        if (errorCode == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE)
        {
            MCP_ERROR("XML", "Failed to get attribute value! Cannot convert xml value to floating point value! Attribute name: '", pAttributeName, "' | Element: ", pElement->Name());
        }

        return result;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Try to get a c-string attribute named pAttributeName from the element. If the attribute is not found, it will return
    ///             the default value with NO message.
    //-----------------------------------------------------------------------------------------------------------------------------
    const char* XMLElement::GetStringAttribute(const char* pAttributeName, const char* defaultVal) const
    {
        const auto* pElement = CAST_TO_CONST_ELEMENT_TYPE(m_pHandle);
        const char* result = pElement->Attribute(pAttributeName, defaultVal);

        return result;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set a bool attribute value. NOTE: This will create an attribute if it does not find one!
    ///		@param pAttributeName : Name of the attribute to set.
    ///		@param val : Value to set the attribute to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void XMLElement::SetBoolAttribute(const char* pAttributeName, const bool val) const
    {
        MCP_CHECK(IsValid());
        CAST_TO_ELEMENT_TYPE(m_pHandle)->SetAttribute(pAttributeName, val);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set a string attribute value. NOTE: This will create an attribute if it does not find one!
    ///		@param pAttributeName : Name of the attribute to set.
    ///		@param val : Value to set the attribute to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void XMLElement::SetStringAttribute(const char* pAttributeName, const char* val) const
    {
        MCP_CHECK(IsValid());
        CAST_TO_ELEMENT_TYPE(m_pHandle)->SetAttribute(pAttributeName, val);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set a Double attribute value. NOTE: This will create an attribute if it does not find one!
    ///		@param pAttributeName : Name of the attribute to set.
    ///		@param val : Value to set the attribute to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void XMLElement::SetDoubleAttribute(const char* pAttributeName, const double val) const
    {
        MCP_CHECK(IsValid());
        CAST_TO_ELEMENT_TYPE(m_pHandle)->SetAttribute(pAttributeName, val);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set a signed int attribute value. NOTE: This will create an attribute if it does not find one!
    ///		@param pAttributeName : Name of the attribute to set.
    ///		@param val : Value to set the attribute to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void XMLElement::SetIntAttribute(const char* pAttributeName, const int64_t val) const
    {
        MCP_CHECK(IsValid());
        CAST_TO_ELEMENT_TYPE(m_pHandle)->SetAttribute(pAttributeName, val);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set an unsigned int attribute value. NOTE: This will create an attribute if it does not find one!
    ///		@param pAttributeName : Name of the attribute to set.
    ///		@param val : Value to set the attribute to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void XMLElement::SetUnsignedIntAttribute(const char* pAttributeName, const uint64_t val) const
    {
        MCP_CHECK(IsValid());
        CAST_TO_ELEMENT_TYPE(m_pHandle)->SetAttribute(pAttributeName, val);
    }

    XMLParser::~XMLParser()
    {
        CloseCurrentFile();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Attempts to load a XML file at the given path and stores it as our working file.
    ///         If the load fails, then it will return false.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool XMLParser::LoadFile(const char* pFilepath)
    {
        m_loadedFile.Load(pFilepath, nullptr, false);
        return m_loadedFile.IsValid();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Load the XML resource.
    //-----------------------------------------------------------------------------------------------------------------------------
    void XMLParser::XMLFile::Load(const char* pFilePath, [[maybe_unused]] const char* pPackageName, [[maybe_unused]] const bool isPersistent)
    {
        if (m_pResource)
        {
            Free();
        }

        m_loadData.pFilePath = pFilePath;
        m_loadData.pPackageName = nullptr;
        m_loadData.isPersistent = false;

        m_pResource = ResourceManager::Get()->Load<tinyxml2::XMLDocument>(m_loadData);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Free the resource.
    //-----------------------------------------------------------------------------------------------------------------------------
    void XMLParser::XMLFile::Free()
    {
        ResourceManager::Get()->FreeResource<tinyxml2::XMLDocument>(m_loadData.pFilePath);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns true if there is currently an XML file loaded in data.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool XMLParser::HasFileLoaded() const
    {
        // Returns if the resource is valid (not freed)
        return m_loadedFile.IsValid();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Close the XML file, freeing the internal resource.\n NOTE: Any Elements that were associated with the xml file
    ///         are now invalid!
    //-----------------------------------------------------------------------------------------------------------------------------
    void XMLParser::CloseCurrentFile()
    {
        m_loadedFile.Free();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the first element from the root document that matches pElementName, or returns an invalid XMLElement if
    ///         no element of the given name was found. If pElementName is nullptr, it will find the first child element.
    ///         \n To check to see if the element is valid, use XMLElement.IsValid().
    //-----------------------------------------------------------------------------------------------------------------------------
    XMLElement XMLParser::GetElement(const char* pElementName) const
    {
        MCP_CHECK_MSG(m_loadedFile.IsValid(), "Failed to get XML Element! No valid file loaded!");

        auto* pOwner = CAST_TO_FILE_TYPE(m_loadedFile.Get());
        auto* pHandle = pOwner->FirstChildElement(pElementName);

        return XMLElement(pHandle);
    }
}