#pragma once
// AudioTrack.h

//#include <vector>
#include "MCP/Application/Resources/Resource.h"

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