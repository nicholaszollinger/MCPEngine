#pragma once
// AudioManager.h

#include "AudioGroup.h"
#include "AudioClip.h"
#include "AudioData.h"
#include "AudioResource.h"
#include "AudioTrack.h"
#include "MCP/Core/System.h"

namespace mcp
{
    class AudioResource;

    class AudioManager final : public System
    {
        MCP_DEFINE_SYSTEM(AudioManager)

        std::unordered_map<AudioGroup::Id, AudioGroup*> m_audioGroups;
        bool m_isMuted = false;

    public:
        void Mute();
        void UnMute();
        [[nodiscard]] bool IsMuted() const;

        AudioGroup* GetGroup(const AudioGroup::Id id);
        AudioGroup* GetGroup(const char* pAudioGroupName);
        AudioHardwareChannel PlayAudio(const AudioResource& resource, const AudioSourceComponent* pAudioSource);

        static AudioManager* Get();
        static AudioManager* AddFromData(const XMLElement) { return BLEACH_NEW(AudioManager); }
    private:
        virtual bool Init() override;
        virtual void Close() override;
        /*void PlayClip(const AudioClip& clip, AudioData& data) const;
        void PlayTrack(const AudioTrack& track, AudioData& data) const;*/
    };
}
