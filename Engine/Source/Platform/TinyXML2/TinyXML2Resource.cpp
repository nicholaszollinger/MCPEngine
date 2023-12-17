#pragma once
// TinyXML2Resource.cpp

#include "MCP/Core/Resource/ResourceManager.h"
#include "tinyxml2.h"

namespace mcp
{
    template<>
    tinyxml2::XMLDocument* ResourceContainer<tinyxml2::XMLDocument, DiskResourceRequest>::LoadFromDiskImpl(const DiskResourceRequest& request)
    {
        auto* pDoc = BLEACH_NEW(tinyxml2::XMLDocument);

        if (pDoc->LoadFile(request.path.GetCStr()) != tinyxml2::XML_SUCCESS)
        {
            MCP_ERROR("XML", "Failed to load xml file at filepath: ", request.path.GetCStr());
            return nullptr;
        }

        return pDoc;
    }
    
    template <>
    tinyxml2::XMLDocument* ResourceContainer<tinyxml2::XMLDocument, DiskResourceRequest>::LoadFromRawDataImpl([[maybe_unused]] char* pRawData, [[maybe_unused]] const int dataSize, [[maybe_unused]] const DiskResourceRequest& request)
    {
        // TODO: There is a way to do it with LoadFile(FILE*), I just need to do testing for it.
        //auto* pDoc = BLEACH_NEW(tinyxml2::XMLDocument);
        //if (pDoc->LoadFile(pRawData) != tinyxml2::XML_SUCCESS)
        MCP_ERROR("XML", "Failed to load xml file from raw data! IMPLEMENTATION NOT DONE.");
        return nullptr;
    }

    template <>
    void ResourceContainer<tinyxml2::XMLDocument, DiskResourceRequest>::FreeResourceImpl(tinyxml2::XMLDocument* pDoc)
    {
        pDoc->Clear();
        BLEACH_DELETE(pDoc);
    }
}
