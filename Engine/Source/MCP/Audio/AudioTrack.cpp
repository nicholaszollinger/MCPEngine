// AudioTrack.cpp

#include "AudioTrack.h"

#include "MCP/Core/Resource/ResourceManager.h"

using KeyType = const char*;

#if MCP_AUDIO_PLATFORM == MCP_AUDIO_PLATFORM_SDL
struct _Mix_Music;

namespace mcp
{
    using AudioTrackType = _Mix_Music;
}
#else
#error "We don't have a resource implementation for Texture loading for current API!'"
#endif

namespace mcp
{
    void* AudioTrack::LoadResourceType()
    {
        return ResourceManager::Get()->LoadFromDisk<AudioTrackType>(m_request);
    }

    void AudioTrack::Free()
    {
        ResourceManager::Get()->FreeResource<AudioTrackType>(m_request);
    }

}
