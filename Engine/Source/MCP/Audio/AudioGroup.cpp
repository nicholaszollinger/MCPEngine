// AudioGroup.cpp

#include "AudioGroup.h"

#include <algorithm>
#include "MCP/Debug/Log.h"
#include "Utility/Generic/Hash.h"

#if MCP_AUDIO_PLATFORM == MCP_AUDIO_PLATFORM_SDL

#pragma warning(push, 0)
#include "Platform/SDL2/SDLAudio.h"
#pragma warning(pop)

using AudioPlatform = SDLAudioManager;

#endif

namespace mcp
{
    AudioGroup::AudioGroup(const char* pName, const float defaultVolume, const bool isMuted, AudioGroup* pParent)
        : m_pParent(pParent)
        , m_id(HashString32(pName))
        , m_volume(defaultVolume)
        , m_isMuted(isMuted)
    {
        // If we have a parent, add ourselves as a child.
        if (m_pParent)
        {
            m_pParent->m_children.emplace_back(this);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      TODO: Potentially need to remove this:
    //		
    ///		@brief : 
    //-----------------------------------------------------------------------------------------------------------------------------
    bool AudioGroup::Init()
    {
        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the volume of this Group.
    //-----------------------------------------------------------------------------------------------------------------------------
    void AudioGroup::SetVolume(const float volume)
    {
        m_volume = std::clamp(volume, 0.f, 1.f);

        // TODO: I need to update any audio resource channels that are currently being played on this group.
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the final volume of this group. The volume of a group is dependent on a parent.
    //-----------------------------------------------------------------------------------------------------------------------------
    float AudioGroup::GetVolume() const
    {
        // Our final volume is dependent on our parents volume.
        if (m_pParent)
        {
            return m_volume * m_pParent->GetVolume();
        }
        
        return m_volume;
    }

    void AudioGroup::Mute()
    {
        m_isMuted = true;

        for (const auto channel : m_channels)
        {
            AudioPlatform::MuteChannel(channel);
        }

        for (auto* pChild : m_children)
        {
            pChild->OnParentMuteChanged(m_isMuted);
        }
    }

    void AudioGroup::UnMute()
    {
        m_isMuted = false;

        for (const auto channel : m_channels)
        {
            //AudioPlatform::UnMuteChannel(channel, );
        }

        for (auto* pChild : m_children)
        {
            pChild->OnParentMuteChanged(m_isMuted);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns if this AudioGroup is muted or not. If a parent group is muted, then this will return true.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool AudioGroup::IsMuted() const
    {
        if (m_pParent && m_pParent->IsMuted())
            return true;

        return m_isMuted;
    }

    void AudioGroup::AddChannel(const AudioHardwareChannel channel)
    {
        // TODO: Make sure that we don't have the channel already.

        m_channels.emplace_back(channel);
    }

    void AudioGroup::RemoveChannel(const AudioHardwareChannel channel)
    {
        for (size_t i = 0; i < m_channels.size(); ++i)
        {
            if (channel == m_channels[i])
            {
                std::swap(m_channels[i], m_channels.back());
                m_channels.pop_back();
                break;
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Pauses all audio sources running on this AudioGroup, and pauses any child AudioGroups
    //-----------------------------------------------------------------------------------------------------------------------------
    void AudioGroup::Pause() const
    {
        for (auto channel : m_channels)
        {
            AudioPlatform::PauseChannel(channel);
        }

        for (auto* pChild : m_children)
        {
            pChild->Pause();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Resumes all audio sources running on this AudioGroup, and resumes all child AudioGroups.
    //-----------------------------------------------------------------------------------------------------------------------------
    void AudioGroup::Resume() const
    {
        for (auto channel : m_channels)
        {
            AudioPlatform::ResumeChannel(channel);
        }

        for (auto* pChild : m_children)
        {
            pChild->Resume();
        }
    }

    // TODO:
    void AudioGroup::OnParentMuteChanged(const bool parentIsMuted)
    {
        // If we are not muted
        if (!m_isMuted)
        {
            // If the parent is now muted,
            if (parentIsMuted)
            {
                // I need to call into the audio platform and mute this
                //AudioPlatform::MuteGroup(static_cast<int>(m_id));
            }

            // If the parent is now un-muted,
            else
            {
                //AudioPlatform::UnMuteGroup(static_cast<int>(m_id));
            }
        }
    }

}
