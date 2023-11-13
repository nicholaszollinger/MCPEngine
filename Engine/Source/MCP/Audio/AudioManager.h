#pragma once
// AudioManager.h

#include "AudioGroup.h"
#include "AudioClip.h"
#include "AudioData.h"
#include "AudioResource.h"
#include "AudioTrack.h"
#include "MCP/Core/GlobalManager.h"

namespace mcp
{
    class AudioResource;

    class AudioManager final : public IProcess
    {
        DEFINE_GLOBAL_MANAGER(AudioManager)

        std::unordered_map<AudioGroup::Id, AudioGroup*> m_audioGroups;
        bool m_isMuted = false;

    public:
        virtual bool Init() override;
        virtual void Close() override;

        void Mute();
        void UnMute();
        [[nodiscard]] bool IsMuted() const;

        AudioGroup* GetGroup(const AudioGroup::Id id);
        AudioGroup* GetGroup(const char* pAudioGroupName);
        AudioHardwareChannel PlayAudio(const AudioResource& resource, const AudioSourceComponent* pAudioSource);

        /*void PlayClip(const AudioClip& clip, AudioData& data) const;
        void PlayTrack(const AudioTrack& track, AudioData& data) const;*/
    };
}