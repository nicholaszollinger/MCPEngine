#pragma once
// SDLAudio.h
#include <cstdint>
#include <vector>

namespace mcp
{
    struct AudioData;
    class AudioClip;
    class AudioTrack;
}

class SDLAudioManager
{
    static inline int s_masterVolume = 0;
    static inline int s_musicVolume = 0;

    // In SDL, there is a single music channel. This value represents the music channel.
    static constexpr int kMusicChannel = -2;
    static constexpr int kInvalidChannel = -1;
    static constexpr float kMaxVolume = 128.f;

    static inline void* pCurrentMusicResource = nullptr;

public:
    static bool Init();
    static void Close();

    // Start of the new interface:
    // General:
    static void MuteChannel(const int channel);
    static void UnMuteChannel(const int channel, const float resumeVolume);
    static void PauseChannel(const int channel);
    static void ResumeChannel(const int channel);

    // Music:
    static int PlayMusic(void* pResource, const float volume, const bool isLooping);

    // Clips:
    static int PlayClip(void* pResource, const float volume, const bool isLooping);

    // End of the new interface:





    // General Audio
    static void Mute();
    static void UnMute();
    static void PauseAudio();
    static void ResumeAudio();

    // Audio Clips
    static int PlayClip(void* pResource, const int volume, const bool isLooping, const int tag);
    static void SetHardwareChannelTag(const int channel, const int tag);
    static void SetChannelVolume(const int channel, const int volume);
    static void ReleaseChannel(const int channel);
    static void PauseGroup(const int tag);
    static void ResumeGroup(const int tag);
    static void MuteGroup(const int tag);
    static void UnMuteGroup(const int tag, const int volume);

    // Music
    static int PlayMusic(void* pResource, const int volume, const bool isLooping, const int tag);
    static void SetMusicVolume(const int volume);
    static void PauseMusic();
    static void ResumeMusic();
    static bool MusicPlayingOrPaused();
    static bool ChannelPlayingOrPaused(const int channel);

private:
    static void PauseChannels();
    static void OnChannelFinished(const int channel);
};