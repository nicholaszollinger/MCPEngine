#pragma once
// AudioSourceComponent.h

#include "Component.h"
#include "MCP/Audio/AudioGroup.h"
#include "MCP/Audio/AudioResource.h"

namespace mcp
{
    struct AudioSourceConstructionData
    {
        const char* pResourcePath = nullptr;
        const char* pAudioGroupName = nullptr;
        float volume = 1.0f;
        bool isLooping = false;
        bool isMuted = false;
        bool playOnActive = false;
        bool isMusicResource = false;
    };

    class AudioSourceComponent final : public Component
    {
        MCP_DEFINE_COMPONENT_ID(AudioSourceComponent)

        AudioResource m_resource;
        AudioGroup::Id m_groupId;
        AudioHardwareChannel m_hardwareChannel;
        float m_volume;
        bool m_isLooping;
        bool m_isMuted;
        bool m_playOnActive;
        // TODO Later: bool m_isSpatial;

    public:
        AudioSourceComponent(const AudioSourceConstructionData& data);

        void Play();
        // TODO: void Stop();
        // TODO: Fading.
        void SetGroup(const AudioGroup::Id id) { m_groupId = id; }
        void SetVolume(const float volume);

        void Mute() { m_isMuted = true; }
        void UnMute() { m_isMuted = false; }

        [[nodiscard]] AudioGroup::Id GetGroupId() const { return m_groupId; }
        [[nodiscard]] bool IsLooping() const { return m_isLooping; }
        [[nodiscard]] bool IsMuted() const { return m_isMuted; }
        [[nodiscard]] float GetVolume() const { return m_volume; }

        static AudioSourceComponent* AddFromData(const XMLElement element);

    private:
        virtual void OnActive() override;
        virtual void OnInactive() override;
    };

}
