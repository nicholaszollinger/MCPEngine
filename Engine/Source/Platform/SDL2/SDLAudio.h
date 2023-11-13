#pragma once
#include <cstdint>
#include <vector>
// SDLAudio.h

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

public:
    static bool Init();
    static void Close();

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
    static void ExpireChannel(const int channel);
};