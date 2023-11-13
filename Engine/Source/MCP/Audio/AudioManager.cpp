// AudioManager.cpp

#include "AudioManager.h"

#include "MCP/Components/AudioSourceComponent.h"
#include "MCP/Core/Config.h"
#include "MCP/Debug/Log.h"

#if MCP_AUDIO_PLATFORM == MCP_AUDIO_PLATFORM_SDL

#pragma warning(push, 0)
#include "Platform/SDL2/SDLAudio.h"
#pragma warning(pop)

using AudioPlatform = SDLAudioManager;

#endif

namespace mcp
{
    bool AudioManager::Init()
    {
        if (!AudioPlatform::Init())
        {
            MCP_ERROR("Audio", "Failed to initialize AudioManager!");
            return false;
        }

        // TODO: I need to create the Master Group.
        auto* pMaster = BLEACH_NEW(AudioGroup("Master", 128)); // TODO: The default master volume should come from data
        m_audioGroups.emplace(pMaster->GetId(), pMaster);

        // Then I need to grab the audio groups from some data file and make them. 'AppProps'? 

        return true;
    }

    void AudioManager::Close()
    {
        // TODO: I need to destroy all of our audio groups.
        for (auto&[id, pGroup] : m_audioGroups)
        {
            BLEACH_DELETE(pGroup);
            pGroup = nullptr;
        }

        AudioPlatform::Close();
    }

    void AudioManager::Mute()
    {
        AudioPlatform::Mute();
        m_isMuted = true;
    }

    void AudioManager::UnMute()
    {
        AudioPlatform::UnMute();
        m_isMuted = false;
    }

    bool AudioManager::IsMuted() const
    {
        return m_isMuted;
    }

    AudioGroup* AudioManager::GetGroup(const AudioGroup::Id id)
    {
        const auto result = m_audioGroups.find(id);
        MCP_CHECK(result != m_audioGroups.end());

        return result->second;
    }

    AudioGroup* AudioManager::GetGroup(const char* pAudioGroupName)
    {
        const auto id = HashString32(pAudioGroupName);
        return GetGroup(id);
    }

    AudioHardwareChannel AudioManager::PlayAudio(const AudioResource& resource, const AudioSourceComponent* pAudioSource)
    {
        // Get the group this audio source belongs to:
        const auto groupId = pAudioSource->GetGroupId();

        auto* pGroup = GetGroup(groupId);
        MCP_CHECK(pGroup);

        // If this group is muted, then return.
        if (pGroup->IsMuted())
            return kNoChannel;

        // Determine what type of audio resource we are attempting to play (Clip or Track)


        // Get the volume of the resource based on the group.
        const auto volume = static_cast<int>(pAudioSource->GetVolume() * static_cast<float>(pGroup->GetVolume()));

        // Call the appropriate function in our AudioPlatform:
        if (resource.IsMusicResource())
        {
            return AudioPlatform::PlayMusic(resource.Get(), volume, pAudioSource->IsLooping(), groupId);
        }

        else
        {
            return AudioPlatform::PlayClip(resource.Get(), volume, pAudioSource->IsLooping(), groupId);
        }
    }

    ////-----------------------------------------------------------------------------------------------------------------------------
    ////		NOTES:
    ////
    /////		@brief : Play an audio clip.
    /////		@param clip : Clip to play.
    /////		@param data : Data defining how it is played.
    ////-----------------------------------------------------------------------------------------------------------------------------
    //void AudioManager::PlayClip(const AudioClip& clip, AudioData& data) const
    //{
    //    AudioPlatform::PlayClip(clip, data);
    //}

    ////-----------------------------------------------------------------------------------------------------------------------------
    ////		NOTES:
    ////
    /////		@brief : Play an audio track.
    /////		@param track : Track to play
    /////		@param data : Data defining how it is played.
    ////-----------------------------------------------------------------------------------------------------------------------------
    //void AudioManager::PlayTrack(const AudioTrack& track, AudioData& data) const
    //{
    //    AudioPlatform::PlayTrack(track, data);
    //}

}