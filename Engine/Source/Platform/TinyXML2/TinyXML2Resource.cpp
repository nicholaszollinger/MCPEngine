#pragma once
// TinyXML2Resource.cpp

#include "MCP/Core/Resource/ResourceManager.h"

#include "tinyxml2.h"

namespace mcp
{
    template<>
    template<>
    tinyxml2::XMLDocument* ResourceContainer<tinyxml2::XMLDocument>::LoadFromDiskImpl(const char* pFilepath)
    {
        auto* pDoc = BLEACH_NEW(tinyxml2::XMLDocument);

        if (pDoc->LoadFile(pFilepath) != tinyxml2::XML_SUCCESS)
        {
            MCP_ERROR("XML", "Failed to load xml file at filepath: ", pFilepath);
            return nullptr;
        }

        return pDoc;
    }

    template <>
    template <>
    tinyxml2::XMLDocument* ResourceContainer<tinyxml2::XMLDocument>::LoadFromRawDataImpl([[maybe_unused]] char* pRawData, [[maybe_unused]] const int dataSize)
    {
        // TODO: There is a way to do it with LoadFile(FILE*), I just need to do testing for it.
        //auto* pDoc = BLEACH_NEW(tinyxml2::XMLDocument);
        //if (pDoc->LoadFile(pRawData) != tinyxml2::XML_SUCCESS)
        MCP_ERROR("XML", "Failed to load xml file from raw data! IMPLEMENTATION NOT DONE.");
        return nullptr;
    }

    template <>
    void ResourceContainer<tinyxml2::XMLDocument>::FreeResourceImpl(tinyxml2::XMLDocument* pDoc)
    {
        pDoc->Clear();
        BLEACH_DELETE(pDoc);
    }
}
