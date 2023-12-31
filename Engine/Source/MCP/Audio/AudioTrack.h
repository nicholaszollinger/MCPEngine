#pragma once
// AudioTrack.h

#include "MCP/Core/Resource/Resource.h"

namespace mcp
{
    class AudioTrack final : public Resource
    {
    public:
        AudioTrack() = default;
        virtual ~AudioTrack() override;

        virtual void Load(const char* pFilePath, const char* pPackageName = nullptr, const bool isPersistent = false) override;
    protected:
        virtual void Free() override;
    };
}