#pragma once
// AudioResource.h

#include "MCP/Core/Resource/Resource.h"
#include "MCP/Core/Config.h"

namespace mcp
{

    struct AudioResourceRequest : public DiskResourceRequest
    {
        bool isMusicResource = false;

        uint64_t operator()(const DiskResourceRequest& request) const
        {
            return std::hash<const void*>{}(request.path.GetConstPtr());
        }

        bool operator==(const DiskResourceRequest& right) const
        {
            return path == right.path;
        }
    };

    struct AudioResourceData
    {
        void* pResource = nullptr;
        bool isMusicResource = false;
    };

    class AudioResource final : public Resource<AudioResourceRequest>
    {
    public:
#if MCP_AUDIO_PLATFORM == MCP_AUDIO_PLATFORM_SDL
        
#endif
        
    public:
        MCP_DEFINE_RESOURCE_DESTRUCTOR(AudioResource)

        [[nodiscard]] virtual void* Get() const override;
        [[nodiscard]] bool IsMusicResource() const;

    protected:
        virtual void Free() override;
        virtual void* LoadResourceType() override;
    };
    
}