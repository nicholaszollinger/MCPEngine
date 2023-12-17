#pragma once
// AudioGroup.h
#include <cstdint>
#include <vector>
#include "MCP/Core/Config.h"

namespace mcp
{
    class AudioSourceComponent;

#if MCP_AUDIO_PLATFORM == MCP_AUDIO_PLATFORM_SDL
    using AudioHardwareChannel = int;
#endif

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      TODO: Audio Groups can be parented. This allows you to have a Master group, and setting its volume will effect all child groups.
    //		
    ///		@brief : An Audio Group is essentially a 'channel' in the final audio that the Audio Listener hears. Audio Groups could be
    ///         things like: Master, Music, Effects, etc. Each Group has a volume that can be set. 
    //-----------------------------------------------------------------------------------------------------------------------------
    class AudioGroup
    {
    public:
        // Id for this AudioGroup, used to get a reference to it from the AudioManager.
        using Id = uint32_t;

        struct PlayAudioData
        {
            AudioGroup* pGroup;                 // The Group that the resource is being played from.
            AudioSourceComponent* pSource;      // The audio source that is currently playing on this Channel
            AudioHardwareChannel channel;       // The channel that this audio source was playing on.
        };

    public:
        static constexpr Id kInvalidId = std::numeric_limits<Id>::max();
        constexpr static AudioHardwareChannel kNoChannel = -1;

    private:
        constexpr static unsigned kDefaultChannelCount = 4;

        std::vector<AudioHardwareChannel> m_channels;   // The channels that are currently playing through this audio group.
        std::vector<AudioGroup*> m_children;
        AudioGroup* m_pParent = nullptr;
        Id m_id;
        float m_volume; // The volume of this Audio Group, from a range of [0, 1]
        bool m_isMuted;

    public:
        AudioGroup(const char* pName, const float defaultVolume, const bool isMuted = false, AudioGroup* pParent = nullptr);
        bool Init();

        // Volume
        void SetVolume(const float volume);
        [[nodiscard]] float GetVolume() const;

        // Mute
        void Mute();
        void UnMute();
        [[nodiscard]] bool IsMuted() const;

        // Play/Pause
        void Pause() const;
        void Resume() const;

        // INTERNAL: Managing active hardware channels
        void AddChannel(const AudioHardwareChannel channel);
        void RemoveChannel(const AudioHardwareChannel channel);

        [[nodiscard]] Id GetId() const  { return m_id; }

    private:
        void OnParentMuteChanged(const bool parentIsMuted);
    };
}