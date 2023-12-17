// SDLAudio.cpp

#include "SDLAudio.h"

#pragma warning(push)
#pragma warning(disable : 26819)
#include <SDL_mixer.h>
#pragma warning(pop)

#include "MCP/Audio/AudioClip.h"
#include "MCP/Audio/AudioManager.h"
#include "MCP/Audio/AudioTrack.h"
#include "MCP/Debug/Log.h"

bool SDLAudioManager::Init()
{
    if(SDL_AudioInit(nullptr) != 0)
    {
        MCP_ERROR("SDL", "Failed to initialize SDL_Audio! SDL_Error: ", SDL_GetError());
        return false;
    }

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) < 0)
    {
        MCP_ERROR("SDL", "Failed to open SDL Mix_Audio: Mix_Error: ", Mix_GetError());
        return false;
    }

    // Initialize Audio format support (MP3 example) // OR more formats if you want to support them.
    int audioFlags = MIX_INIT_MP3;
    if (Mix_Init(audioFlags) != audioFlags)
    {
        MCP_ERROR("SDL", "Failed to initialize SDL_Mixer! Mix_Error: ", Mix_GetError());
        return false;
    }

    // Set our channels to expire when they finish.
    Mix_ChannelFinished(&SDLAudioManager::OnChannelFinished);

    return true;
}

void SDLAudioManager::Close()
{
    Mix_CloseAudio();
}

void SDLAudioManager::PauseChannel(const int channel)
{
    if (channel == kMusicChannel)
    {
        Mix_PauseMusic();
    }

    else
        Mix_Pause(channel);
}

void SDLAudioManager::ResumeChannel(const int channel)
{
    if (channel == kMusicChannel)
    {
        Mix_ResumeMusic();
    }

    else
        Mix_Resume(channel);
}

void SDLAudioManager::MuteChannel(const int channel)
{
    if (channel == kMusicChannel)
    {
        Mix_VolumeMusic(0);
    }

    else
    {
        Mix_Volume(channel, 0);
    }
}

void SDLAudioManager::UnMuteChannel(const int channel, const float resumeVolume)
{
    if (channel == kMusicChannel)
    {
        Mix_VolumeMusic(static_cast<int>(resumeVolume * kMaxVolume));
    }

    else
    {
        Mix_Volume(channel, static_cast<int>(resumeVolume * kMaxVolume));
    }
}

int SDLAudioManager::PlayMusic(void* pResource, const float volume, const bool isLooping)
{
    auto* pMusic = static_cast<Mix_Music*>(pResource);

    // For now, just play the music.
    if (Mix_PlayMusic(pMusic, isLooping? -1 : 0) != 0)
    {
        return kInvalidChannel;    
    }

    Mix_VolumeMusic(static_cast<int>(volume * kMaxVolume));

    // TODO: Set up the fades:
    // If there is music playing, we need to fade out the current music and set the hook to play
    // this next track.
    //if (Mix_FadingMusic() != MIX_NO_FADING)
    //{

    //    
    //    //Mix_HookMusicFinished()
    //}

    //// If there is no playing music, we can begin playing our next track
    //else
    //{
    //}

    return kMusicChannel;
}

int SDLAudioManager::PlayClip(void* pResource, const float volume, const bool isLooping)
{
    auto* pChunk = static_cast<Mix_Chunk*>(pResource);

    Mix_VolumeChunk(pChunk, static_cast<int>(volume * kMaxVolume));

    const auto channel = Mix_PlayChannel(-1, pChunk, isLooping? -1 : 0);

    return channel;
}

void SDLAudioManager::Mute()
{
    s_musicVolume = Mix_VolumeMusic(0);
}

void SDLAudioManager::UnMute()
{
    Mix_VolumeMusic(s_musicVolume);
}

int SDLAudioManager::PlayClip(void* pResource, const int volume, const bool isLooping, const int tag)
{
    auto* pChunk = static_cast<Mix_Chunk*>(pResource);

    // Set the volume
    Mix_VolumeChunk(pChunk, volume);

    // Play it on an available channel.
    const auto channel = Mix_PlayChannel(-1, pChunk, isLooping? -1 : 0);

    // Set the tag for this channel.
    SetHardwareChannelTag(channel, tag);

    return channel;
}

void SDLAudioManager::PauseAudio()
{
    SDL_PauseAudio(1);
}

void SDLAudioManager::ResumeAudio()
{
    SDL_PauseAudio(0);
}

void SDLAudioManager::MuteGroup([[maybe_unused]] const int tag)
{
    // Set all channels with this tag to 
}

void SDLAudioManager::UnMuteGroup([[maybe_unused]] const int tag, [[maybe_unused]] const int volume)
{
    // TODO:
}


int SDLAudioManager::PlayMusic([[maybe_unused]] void* pResource, [[maybe_unused]] const int volume, [[maybe_unused]] const bool isLooping, [[maybe_unused]] const int tag)
{
    auto* pMusic = static_cast<Mix_Music*>(pResource);

    // Set the volume
    Mix_VolumeMusic(volume);

    // Play it on an available channel.
    if (Mix_PlayMusic(pMusic, isLooping? -1 : 0) == 0)
    {
        MCP_ERROR("SDL", "Failed to play music!");
    }

    return -1;
}

void SDLAudioManager::ReleaseChannel(const int channel)
{
    // If our channel is currently playing, we are going to fade it out, then it will release once complete.
    if (ChannelPlayingOrPaused(channel))
    {
        Mix_FadeOutChannel(channel, 1000);
    }

    // If nothing is playing right now, then expire the channel immediately.
    else
    {
        OnChannelFinished(channel);   
    }
}

void SDLAudioManager::SetChannelVolume(const int channel, const int volume)
{
    if (ChannelPlayingOrPaused(channel))
    {
        Mix_Volume(channel, volume);
    }
}

void SDLAudioManager::SetMusicVolume(const int volume)
{
    // If we have music playing,
    if (MusicPlayingOrPaused())
    {
        Mix_VolumeMusic(volume);
    }
}

bool SDLAudioManager::MusicPlayingOrPaused()
{
    return Mix_PlayingMusic() != 0;
}

bool SDLAudioManager::ChannelPlayingOrPaused(const int channel)
{
    if (channel == -1)
        return false;

    return Mix_Playing(channel);
}

void SDLAudioManager::SetHardwareChannelTag(const int channel, const int tag)
{
    if (Mix_GroupChannel(channel, tag) == 0)
    {
        MCP_ERROR("SDL", "Failed to set tag for audio channel!");
    }
}

void SDLAudioManager::PauseChannels()
{
    Mix_Pause(-1);
}

void SDLAudioManager::PauseMusic()
{
    Mix_PauseMusic();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Callback for when a Channel completes.
//-----------------------------------------------------------------------------------------------------------------------------
void SDLAudioManager::OnChannelFinished(const int channel)
{
    mcp::AudioManager::Get()->Internal_OnChannelFinished(channel);
}
