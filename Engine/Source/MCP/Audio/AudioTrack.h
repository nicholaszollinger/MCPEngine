#pragma once
// AudioTrack.h

#include "MCP/Core/Resource/Resource.h"

namespace mcp
{
    class AudioTrack final : public DiskResource
    {
    public:
        MCP_DEFINE_RESOURCE_DESTRUCTOR(AudioTrack)

    protected:
        virtual void* LoadResourceType() override;
        virtual void Free() override;
    };
}