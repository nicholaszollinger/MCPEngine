// AudioClip.cpp

#include "AudioClip.h"

#include "MCP/Core/Resource/ResourceManager.h"

using KeyType = const char*;

#if MCP_AUDIO_PLATFORM == MCP_AUDIO_PLATFORM_SDL
struct Mix_Chunk;

namespace mcp
{
    using AudioClipType = Mix_Chunk;
}
#else
#error "We don't have a resource implementation for Texture loading for current API!'"
#endif

namespace mcp
{
    void* AudioClip::LoadResourceType()
    {
        return ResourceManager::Get()->LoadFromDisk<AudioClipType>(m_request);
    }

    void AudioClip::Free()
    {
        ResourceManager::Get()->FreeResource<AudioClipType>(m_request);
    }
}