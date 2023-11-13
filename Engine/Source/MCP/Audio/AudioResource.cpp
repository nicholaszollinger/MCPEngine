// AudioResource.cpp

#include "AudioResource.h"

#include "MCP/Core/Resource/ResourceManager.h"

namespace mcp
{
    void* AudioResource::LoadResourceType()
    {
        return ResourceManager::Get()->LoadFromDisk<AudioResourceData>(m_request);
    }

    void AudioResource::Free()
    {
        ResourceManager::Get()->FreeResource<AudioResourceData>(m_request);
    }

    void* AudioResource::Get() const
    {
        if (!m_pResource)
            return nullptr;

         const auto* pAudioResourceData = static_cast<AudioResourceData*>(m_pResource);
        return pAudioResourceData->pResource;
    }

    bool AudioResource::IsMusicResource() const
    {
        const auto* pAudioResourceData = static_cast<AudioResourceData*>(m_pResource);
        return pAudioResourceData->isMusicResource;
    }
}
