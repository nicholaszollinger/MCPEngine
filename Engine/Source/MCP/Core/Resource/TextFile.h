#pragma once
// TextFile.h

#include "Resource.h"
// This is a test for the compress and expand functions.

namespace mcp
{
    class TextFile final : public DiskResource
    {
    public:
        MCP_DEFINE_RESOURCE_DESTRUCTOR(TextFile)

        void Dump() const;

    protected:
        virtual void* LoadResourceType() override;
        virtual void Free() override;
    };
}
