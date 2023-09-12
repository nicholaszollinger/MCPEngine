#pragma once
// AudioClip.h

#include "MCP/Core/Resource/Resource.h"

namespace mcp
{
    class AudioClip final : public Resource
    {
    public:
        AudioClip() = default;
        virtual ~AudioClip() override;

        virtual void Load(const char* pFilePath, const char* pPackageName = nullptr, const bool isPersistent = false) override;
    protected:
        virtual void Free() override;
    };
}