#pragma once
// CollisionProfile.h

#include "CollisionChannels.h"
#include "utility/EnumHelpers.h"

enum class CollisionResponse
{
    kIgnore = Bit<uint8_t>(0), // The Collider will ignore all collisions on that channel.
    kOverlap = Bit<uint8_t>(1), // The Collider will not collide physically, but will send overlap events.
    kBlock = Bit<uint8_t>(2),   // The Collider will hit and be blocked by other colliders in this channel and send hit events.
};

// This allows me to use the above enum with more ease.
DEFINE_ENUM_CLASS_BITWISE_OPERATORS(CollisionResponse)

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : The Collision profile defines a Collider's channel that it is in as well as all of the responses it has toward
    ///         other channels.
    //-----------------------------------------------------------------------------------------------------------------------------
    struct CollisionProfile
    {
        CollisionResponse collisionResponses[Internal::kMaxChannels];
        CollisionChannel myCollisionChannel;

        CollisionProfile();
    };
}