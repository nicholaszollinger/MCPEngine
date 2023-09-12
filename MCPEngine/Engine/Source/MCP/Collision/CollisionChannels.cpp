// CollisionChannels.cpp

#include "CollisionChannels.h"

#include <cassert>
#include "MCP/Debug/Log.h"
#include "Utility/Generic/Hash.h"

namespace mcp::Internal
{
    static constexpr uint32_t kDefaultChannelHashed = HashString32(kDefaultChannelName);

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Either return the collision channel that is already mapped to the given name, or define a new mapping and return
    ///             that channel.
    //-----------------------------------------------------------------------------------------------------------------------------
    CollisionChannel CollisionChannelManager::GetOrAssignCollisionChannel(const char* pChannelName)
    {
        const uint32_t hashedName = HashString32(pChannelName);

        // Check for our only reserved channel.
        if (hashedName == kDefaultChannelHashed)
            return CollisionChannel::kDefault;

        // See if we already have a channel mapped to that name.
        const auto result = m_hashedNamesToEnumIndex.find(hashedName);
        if ( result != m_hashedNamesToEnumIndex.end())
        {
            return static_cast<CollisionChannel>(m_hashedNamesToEnumIndex[hashedName]);
        }

        // If we don't have it, we are going to define a new one.
        // If we have already defined the maximum amount, we need to throw an error.
        if (m_channelsDefined >= kMaxChannels)
        {
            LogError("Failed to assign name to Collision Channel! Exceeded the max allowable channel mappings!");
            assert(false); // This error should be caught in debug.
            return CollisionChannel::kChannel1; // Dummy return.
        }

        m_hashedNamesToEnumIndex[hashedName] = m_channelsDefined;
        ++m_channelsDefined;

        return static_cast<CollisionChannel>(m_channelsDefined - 1);
    }
}
