// AudioGroup.cpp

#include "AudioGroup.h"

#include <algorithm>
#include "Utility/Generic/Hash.h"

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
    AudioGroup::AudioGroup(const char* pName, const int defaultVolume, const bool isMuted, AudioGroup* pParent)
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
    //		
    ///		@brief : Get the final volume of this group. The volume of a group is dependent on a parent.
    //-----------------------------------------------------------------------------------------------------------------------------
    int AudioGroup::GetVolume() const
    {
        // Our final volume is dependent on our parents volume.
        if (m_pParent)
        {
            const auto parentVolume = m_pParent->GetVolume();
            return static_cast<int>(GetNormalizedVolume() * static_cast<float>(parentVolume));
        }
        
        return m_volume;
    }

    void AudioGroup::Mute()
    {
        m_isMuted = true;

        for (auto* pChild : m_children)
        {
            pChild->OnParentMuteChanged(m_isMuted);
        }
    }

    void AudioGroup::UnMute()
    {
        m_isMuted = false;

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

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the volume of this Group.
    //-----------------------------------------------------------------------------------------------------------------------------
    void AudioGroup::SetVolume(const int volume)
    {
        m_volume = std::clamp(volume, kMinVolume, kMaxVolume);
    }

    float AudioGroup::GetNormalizedVolume() const
    {
        return static_cast<float>(m_volume) / static_cast<float>(kMaxVolume);
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
