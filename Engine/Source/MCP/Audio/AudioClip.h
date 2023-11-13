#pragma once
// AudioClip.h

#include "MCP/Core/Resource/Resource.h"

namespace mcp
{
    class AudioClip final : public DiskResource
    {
    public:
        MCP_DEFINE_RESOURCE_DESTRUCTOR(AudioClip)

    protected:
        virtual void* LoadResourceType() override;
        virtual void Free() override;
    };
}