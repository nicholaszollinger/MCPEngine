// AudioTrack.cpp

#include "AudioTrack.h"

#include "MCP/Application/Resources/ResourceManager.h"

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

    AudioTrack::~AudioTrack()
    {
        if (m_pResource)
        {
            Free();
        }
    }

    void AudioTrack::Load(const char* pFilePath, const char* pPackageName, const bool isPersistent)
    {
        if (m_pResource)
        {
            Free();
        }

        m_loadData.pFilePath = pFilePath;
        m_loadData.pPackageName = pPackageName;
        m_loadData.isPersistent = isPersistent;

        m_pResource = ResourceManager::Get()->Load<AudioTrackType>(m_loadData);
    }

    void AudioTrack::Free()
    {
        ResourceManager::Get()->FreeResource<AudioTrackType>(m_loadData.pFilePath);
    }

}
