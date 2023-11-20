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
        if (data.pResourcePath)
        {
            AudioResourceRequest request;
            request.path = data.pResourcePath;
            request.isMusicResource = data.isMusicResource;

            if (!m_resource.Load(request))
            {
                MCP_ERROR("AudioSourceComponent", "Failed to load AudioResource! Path: ", data.pResourcePath);
            }
        }

        const auto* pGroup = AudioManager::Get()->GetGroup(data.pAudioGroupName);
        MCP_CHECK(pGroup);

        m_groupId = pGroup->GetId();
    }

    void AudioSourceComponent::Play()
    {
        m_hardwareChannel = AudioManager::Get()->PlayAudio(this, m_resource, m_groupId, m_volume);
    }

    void AudioSourceComponent::SetVolume(const float volume)
    {
        m_volume = std::clamp(volume, 0.f, 1.f);
    }

    void AudioSourceComponent::OnActive()
    {
        if (m_playOnActive)
            Play();
    }

    void AudioSourceComponent::OnInactive()
    {
        // TODO: Remove this channel:
    }

    AudioSourceComponent* AudioSourceComponent::AddFromData(const XMLElement element)
    {
        AudioSourceConstructionData data;

        data.pAudioGroupName = element.GetAttributeValue<const char*>("group", "Master");
        data.volume = element.GetAttributeValue<float>("volume", 1.f);
        data.isLooping = element.GetAttributeValue<bool>("isLooping", false);
        data.isMuted = element.GetAttributeValue<bool>("isMuted", false);
        data.playOnActive = element.GetAttributeValue<bool>("playOnActive", false);

        const auto resourceElement = element.GetChildElement("Resource");
        if (resourceElement.IsValid())
        {
            data.pResourcePath = resourceElement.GetAttributeValue<const char*>("path", nullptr);
            data.isMusicResource = resourceElement.GetAttributeValue<bool>("isMusic", false);
        }

        return BLEACH_NEW(AudioSourceComponent(data));
    }


}
