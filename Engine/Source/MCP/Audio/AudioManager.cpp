// AudioManager.cpp

#include "AudioManager.h"

#include "MCP/Components/AudioSourceComponent.h"
#include "MCP/Core/Config.h"
#include "MCP/Debug/Log.h"
#include "MCP/Core/Application/Application.h"

#if MCP_AUDIO_PLATFORM == MCP_AUDIO_PLATFORM_SDL
// Disable SDL Warnings.
#pragma warning(push, 0)
#include "Platform/SDL2/SDLAudio.h"
#pragma warning(pop)
using AudioPlatform = SDLAudioManager;
#endif

namespace mcp
{
    MCP_DEFINE_STATIC_SYSTEM_GETTER(AudioManager)

    AudioManager::AudioManager(AudioGroupContainer&& audioGroups)
        : m_audioGroups(std::move(audioGroups))
    {
        //
    }

    bool AudioManager::Init()
    {
        if (!AudioPlatform::Init())
        {
            MCP_ERROR("Audio", "Failed to initialize AudioManager!");
            return false;
        }

        return true;
    }

    void AudioManager::Close()
    {
        // Destroy all of our audio groups.
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
        if (result == m_audioGroups.end())
            return nullptr;

        return result->second;
    }

    AudioGroup* AudioManager::GetGroup(const char* pAudioGroupName)
    {
        const auto id = HashString32(pAudioGroupName);
        return GetGroup(id);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the id for the AudioGroup named 'pAudioGroupName'.
    ///		@param pAudioGroupName : Name of the Audio Group you are trying to find.
    ///		@returns : Id of the AudioGroup, or AudioGroup::kInvalidId if no group is found.
    //-----------------------------------------------------------------------------------------------------------------------------
    AudioGroup::Id AudioManager::GetGroupId(const char* pAudioGroupName)
    {
        const auto* pGroup = GetGroup(pAudioGroupName);
        if (!pGroup)
            return AudioGroup::kInvalidId;

        return pGroup->GetId();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Play a piece of Audio.
    ///		@param pSource : The audio source component that is playing the piece of audio.
    ///		@param resource : The Resource we are going to play
    ///		@param groupId : The Id of the AudioGroup that this Audio Source belongs to.
    ///		@param volume : The volume to play the resource at.
    ///		@returns : The hardware channel that this Audio Resource is playing on.
    //-----------------------------------------------------------------------------------------------------------------------------
    AudioHardwareChannel AudioManager::PlayAudio(const AudioSourceComponent* pSource, const AudioResource& resource, const AudioGroup::Id groupId, const float volume)
    {
        auto* pGroup = GetGroup(groupId);
        MCP_CHECK(pGroup);

        PlayingAudioData data;
        data.groupId = groupId;
        data.volume = pGroup->GetVolume() * volume;

        if (resource.IsMusicResource())
        {
            data.channel = AudioPlatform::PlayMusic(resource.Get(), data.volume, pSource->IsLooping());
        }

        else
        {
            data.channel = AudioPlatform::PlayClip(resource.Get(), data.volume, pSource->IsLooping());
        }

        // Add the data to our currently playing audio:
        m_activeChannels.emplace_back(data);

        // Add the channel to the group:
        pGroup->AddChannel(data.channel);

        // Return the channel
        return data.channel;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : INTERNAL FUNCTION. Called when a audio resource finishes playing on a Channel.
    //-----------------------------------------------------------------------------------------------------------------------------
    void AudioManager::Internal_OnChannelFinished(const AudioHardwareChannel channel)
    {
        for (size_t i = 0; i < m_activeChannels.size(); ++i)
        {
            const auto& data = m_activeChannels[i];

            if (channel == data.channel)
            {
                // Remove the AudioGroups channel
                auto* pGroup = m_audioGroups[data.groupId];
                MCP_CHECK(pGroup);

                pGroup->RemoveChannel(channel);

                // Remove this audio data:
                std::swap(m_activeChannels[i], m_activeChannels.back());
                m_activeChannels.pop_back();
                return;
            }
        }
    }

    AudioManager* AudioManager::AddFromData(const XMLElement element)
    {
        const auto audioElement = element.GetChildElement("Audio");
        if (!audioElement.IsValid())
        {
            MCP_ERROR("AudioManager", "Failed to create AudioManager from data! No 'Audio' element found in the Project Settings");
            return nullptr;
        }

        // Create the Audio Groups:
        AudioGroupContainer audioGroups;

        // MasterVolume
        auto* pMaster = BLEACH_NEW(AudioGroup(kMasterVolumeName, 1.f));
        audioGroups.emplace(pMaster->GetId(), pMaster);

        auto childElement = audioElement.GetChildElement("MasterVolume");
        if (childElement.IsValid())
        {
            const auto masterVolume = childElement.GetAttributeValue<float>("volume", 1.f);
            pMaster->SetVolume(masterVolume);

            if (childElement.GetAttributeValue<bool>("isMuted", false))
                pMaster->Mute();
        }

        // Audio Groups:
        childElement = audioElement.GetChildElement("AudioGroups");
        if (childElement.IsValid())
        {
            auto groupElement = childElement.GetChildElement("Group");

            while (groupElement.IsValid())
            {
                CreateChildAudioGroupFromData(audioGroups, pMaster, groupElement);

                groupElement = groupElement.GetSiblingElement("Group");
            }
        }

        return BLEACH_NEW(AudioManager(std::move(audioGroups)));
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Creates, recursively, each Audio Group and parents them accordingly.
    //-----------------------------------------------------------------------------------------------------------------------------
    void AudioManager::CreateChildAudioGroupFromData(AudioGroupContainer& container, AudioGroup* pParent, const XMLElement groupElement)
    {
        // Get the AudioGroup Data:
        const auto name = groupElement.GetAttributeValue<const char*>("name", nullptr);
        // TODO: Handle the error of no name
        MCP_CHECK(name);

        const auto volume = groupElement.GetAttributeValue<float>("volume", 1.f);
        const auto isMuted = groupElement.GetAttributeValue<bool>("isMuted", false);

        // Create the Group as a child of pParent
        auto* pNewGroup = BLEACH_NEW(AudioGroup(name, volume, isMuted, pParent));

        // Add the group to our container
        container.emplace(pNewGroup->GetId(), pNewGroup);

        // Load any child Audio Groups:
        auto childGroup = groupElement.GetChildElement("Group");
        while (childGroup.IsValid())
        {
            CreateChildAudioGroupFromData(container, pNewGroup, childGroup);
            childGroup = childGroup.GetSiblingElement("Group");
        }
    }
}