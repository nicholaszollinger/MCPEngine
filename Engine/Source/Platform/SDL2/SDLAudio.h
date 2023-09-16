#pragma once
// SDLAudio.h

namespace mcp
{
    struct AudioData;
    class AudioClip;
    class AudioTrack;
}

class SDLAudioManager
{
public:
    static bool Init();
    static void Close();
    static void PlayClip(const mcp::AudioClip& clip, mcp::AudioData& data);
    static void PlayTrack(const mcp::AudioTrack& track, mcp::AudioData& data);

private:
    static void SetClipVolume(const mcp::AudioClip& clip, const mcp::AudioData& data);
};