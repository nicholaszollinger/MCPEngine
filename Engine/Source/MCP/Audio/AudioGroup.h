#pragma once
#include <cstdint>
#include <vector>
// AudioGroup.h

namespace mcp
{
    class AudioSourceComponent;

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
        using Id = uint32_t;

        static constexpr int kMaxVolume = 128;
        static constexpr int kMinVolume = 0;

    private:
        std::vector<AudioGroup*> m_children;
        AudioGroup* m_pParent = nullptr;
        Id m_id;
        int m_volume; // The volume of this Audio Group, from a range of [0, 128]
        bool m_isMuted;

    public:
        AudioGroup(const char* pName, const int defaultVolume, const bool isMuted = false, AudioGroup* pParent = nullptr);

        void SetVolume(const int volume);

        void Mute();
        void UnMute();
        [[nodiscard]] bool IsMuted() const;
        [[nodiscard]] int GetVolume() const;
        [[nodiscard]] Id GetId() const  { return m_id; }

    private:
        void OnParentMuteChanged(const bool parentIsMuted);
        [[nodiscard]] float GetNormalizedVolume() const;
    };
}