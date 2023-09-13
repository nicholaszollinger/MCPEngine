// SDLAudio.cpp

#include "SDLAudio.h"

#pragma warning(push)
#pragma warning(disable : 26819)
#include <SDL_mixer.h>
#pragma warning(pop)

#include "MCP/Audio/AudioClip.h"
#include "MCP/Audio/AudioData.h"
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

    return true;
}

void SDLAudioManager::Close()
{
    Mix_CloseAudio();
}

void SDLAudioManager::PlayClip(const mcp::AudioClip& clip, mcp::AudioData& data)
{
    SetClipVolume(clip, data);
    data.channel = Mix_PlayChannel(data.channel, static_cast<Mix_Chunk*>(clip.Get()), data.loops);
}

void SDLAudioManager::PlayTrack(const mcp::AudioTrack& track, mcp::AudioData& data)
{
    // TODO: Set up how to handle music fades.

    data.channel = Mix_PlayMusic(static_cast<Mix_Music*>(track.Get()), data.loops);
    Mix_VolumeMusic(data.volume);
}

void SDLAudioManager::SetClipVolume(const mcp::AudioClip& clip, const mcp::AudioData& data)
{
    Mix_VolumeChunk(static_cast<Mix_Chunk*>(clip.Get()), data.volume);
}