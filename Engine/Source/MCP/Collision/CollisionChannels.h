#pragma once
// CollisionChannels.h

#include <unordered_map>

namespace mcp
{
    // The name of the first and only reserved collision channel.
    static constexpr const char* kDefaultChannelName = "Default";

    enum class CollisionChannel
    {
        kDefault,   // Only reserved Channel. All colliders' 'MyCollisionChannel' defaults to this.
                    // Every other channel can have a user defined name.
        kChannel1,
        kChannel2,
        kChannel3,
        kChannel4,
        kChannel5,
        kChannel6,
        kChannel7,
        kChannel8,
        kChannel9,
        kChannel10,
        kChannel11,
        kChannel12,
        kChannel13,
        kChannel14,
        kChannel15,
        kChannel16,
        kChannel17,
        kChannel18,
        kChannel19,
        kChannel20,
        kChannel21,
        kChannel22,
        kChannel23,
        kChannel24,
        kChannel25,
        kChannel26,
        kChannel27,
        kChannel28,
        kChannel29,
        kChannel30,
        kChannel31,
    };

    namespace Internal
    {
        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //		
        ///		@brief : This class will serve as a repository for named mappings to collision channels.
        ///         I wanted the user to be able to define what the channels are called, and be able to interact with
        ///         channels by name. GetOrAssignCollisionChannel will either return a previously made mapping or create a
        ///         new mapping and return the channel. This class is to be used internally to Colliders. The client should have
        ///         to worry about this directly.
        //-----------------------------------------------------------------------------------------------------------------------------
        class CollisionChannelManager
        {
            static inline std::unordered_map<uint32_t, size_t> m_hashedNamesToEnumIndex;
            static inline size_t m_channelsDefined = 1;

        public:
            static constexpr size_t kMaxChannels = 32;

        public:
            static CollisionChannel GetOrAssignCollisionChannel(const char* pChannelName);
        };
    }
    
}