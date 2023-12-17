#pragma once
// AudioManager.h

#include "AudioGroup.h"
#include "AudioClip.h"
#include "AudioResource.h"
#include "AudioTrack.h"
#include "MCP/Core/System.h"

namespace mcp
{
    class AudioResource;

    class AudioManager final : public System
    {
    private:
        struct PlayingAudioData
        {
            AudioGroup::Id groupId = AudioGroup::kInvalidId;
            AudioHardwareChannel channel = AudioGroup::kNoChannel;
            float volume = 0.f;
        };

        using AudioGroupContainer = std::unordered_map<AudioGroup::Id, AudioGroup*>;
        using ActiveChannelContainer = std::vector<PlayingAudioData>;

    private:
        MCP_DEFINE_SYSTEM(AudioManager)

        static constexpr const char* kMasterVolumeName = "Master";
        AudioGroupContainer m_audioGroups;
        ActiveChannelContainer m_activeChannels;
        bool m_isMuted = false;

        // Private Ctor
        AudioManager(AudioGroupContainer&& audioGroups);

    public:
        void Mute();
        void UnMute();
        [[nodiscard]] bool IsMuted() const;

        AudioGroup* GetGroup(const AudioGroup::Id id);
        AudioGroup* GetGroup(const char* pAudioGroupName);
        AudioGroup::Id GetGroupId(const char* pAudioGroupName);
        AudioHardwareChannel PlayAudio(const AudioSourceComponent* pSource, const AudioResource& resource, const AudioGroup::Id groupId, const float volume);

        // Internal:
        void Internal_OnChannelFinished(const AudioHardwareChannel channel);

        static AudioManager* Get();
        static AudioManager* AddFromData(const XMLElement);

    private:
        static void CreateChildAudioGroupFromData(AudioGroupContainer& container, AudioGroup* pParent, const XMLElement groupElement);

        virtual bool Init() override;
        virtual void Close() override;
    };
}
