// AudioClip.cpp

#include "AudioClip.h"

#include "MCP/Core/Resource/ResourceManager.h"

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
    AudioClip::~AudioClip()
    {
        if (m_pResource)
        {
            Free();
        }
    }

    void AudioClip::Load(const char* pFilePath, const char* pPackageName, const bool isPersistent)
    {
        if (m_pResource)
        {
            Free();
        }

        m_loadData.pFilePath = pFilePath;
        m_loadData.pPackageName = pPackageName;
        m_loadData.isPersistent = isPersistent;

        m_pResource = ResourceManager::Get()->Load<AudioClipType>(m_loadData);
    }

    void AudioClip::Free()
    {
        ResourceManager::Get()->FreeResource<AudioClipType>(m_loadData.pFilePath);
    }
}