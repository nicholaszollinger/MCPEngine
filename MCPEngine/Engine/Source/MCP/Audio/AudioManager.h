#pragma once
// AudioManager.h

//#include <vector>
#include "AudioClip.h"
#include "AudioData.h"
#include "AudioTrack.h"
#include "MCP/Application/Core/GlobalManager.h"

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
    //void PlayTrackList()
}