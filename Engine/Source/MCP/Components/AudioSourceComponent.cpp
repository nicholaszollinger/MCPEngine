// AudioSourceComponent.cpp

#include "AudioSourceComponent.h"

#include <algorithm>

#include "MCP/Audio/AudioManager.h"

namespace mcp
{
    AudioSourceComponent::AudioSourceComponent(const AudioSourceConstructionData& data)
        : Component(true)
        , m_groupId(0)
        , m_hardwareChannel(-1)
        , m_volume(data.volume)
        , m_isLooping(data.isLooping)
        , m_isMuted(data.isMuted)
        , m_playOnActive(data.playOnActive)
    {
        //
    }

    void AudioSourceComponent::Play()
    {
        if (m_isMuted)
            return;

        m_hardwareChannel = AudioManager::Get()->PlayAudio(m_resource, this);
    }

    void AudioSourceComponent::SetVolume(const float volume)
    {
        m_volume = std::clamp(volume, 0.f, 1.f);
    }

    AudioSourceComponent* AudioSourceComponent::AddFromData([[maybe_unused]] const XMLElement element)
    {
        AudioSourceConstructionData data;

        return BLEACH_NEW(AudioSourceComponent(data));
    }


}
