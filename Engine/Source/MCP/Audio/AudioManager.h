#pragma once
// AudioManager.h

#include "AudioClip.h"
#include "AudioData.h"
#include "AudioTrack.h"
#include "MCP/Core/GlobalManager.h"

namespace mcp
{
    class AudioManager final : public IProcess
    {
        DEFINE_GLOBAL_MANAGER(AudioManager)

    public:
        virtual bool Init() override;
        virtual void Close() override;
    };

    void PlayClip(const AudioClip& clip, AudioData& data);
    void PlayTrack(const AudioTrack& track, AudioData& data);
}