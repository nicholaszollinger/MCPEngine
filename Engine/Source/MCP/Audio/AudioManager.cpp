// AudioManager.cpp

#include "AudioManager.h"

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

        return true;
    }

    void AudioManager::Close()
    {
        AudioPlatform::Close();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Play an audio clip.
    ///		@param clip : Clip to play.
    ///		@param data : Data defining how it is played.
    //-----------------------------------------------------------------------------------------------------------------------------
    void PlayClip(const AudioClip& clip, AudioData& data)
    {
        AudioPlatform::PlayClip(clip, data);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Play an audio track.
    ///		@param track : Track to play
    ///		@param data : Data defining how it is played.
    //-----------------------------------------------------------------------------------------------------------------------------
    void PlayTrack(const AudioTrack& track, AudioData& data)
    {
        AudioPlatform::PlayTrack(track, data);
    }

}