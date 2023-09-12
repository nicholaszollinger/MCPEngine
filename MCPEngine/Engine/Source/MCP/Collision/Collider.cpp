// Collider.h

#include "Collider.h"

#include "CollisionSystem.h"
#include "MCP/Components/ColliderComponent.h"
#include "Utility/Generic/Hash.h"

namespace mcp
{
    Collider::Collider(const char* name, const bool isEnabled, const Vec2 position)
        : m_relativePosition(position)
        , m_pSystem(nullptr)
        , m_pOwner(nullptr)
        , m_colliderName(HashString32(name))
        , m_isEnabled(isEnabled)
    {
        // By default, we are going to be blocking all Channels, and a part of the first channel.
        SetMyCollisionChannel(CollisionChannel::kDefault);
        SetCollisionResponseToAllChannels(CollisionResponse::kBlock);

        // Reserve some space for a reasonable amount of overlaps.
        m_overlappingColliders.reserve(4);
    }

    void Collider::SetCollisionEnabled(const bool isEnabled)
    {
        // If our state isn't changing, return.
        if (isEnabled == m_isEnabled)
            return;

        m_isEnabled = isEnabled;
    }

    Vec2 Collider::GetWorldCenter() const
    {
        const Vec2 ownerPos = m_pOwner->GetTransformComponent()->GetLocation();
        return ownerPos + m_relativePosition;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the collision channel that this Collider will be representing.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Collider::SetMyCollisionChannel(const CollisionChannel channel)
    {
        m_profile.myCollisionChannel = channel;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the collision channel that this Collider will be representing by a user-defined name.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Collider::SetMyCollisionChannel(const char* pChannelName)
    {
        const auto channel = Internal::CollisionChannelManager::GetOrAssignCollisionChannel(pChannelName);
        m_profile.myCollisionChannel = channel;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the collision response for a specific channel.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Collider::SetCollisionResponseToChannel(const CollisionChannel channel, const CollisionResponse response)
    {
        const size_t channelIndex = static_cast<size_t>(channel);
        assert(channelIndex < Internal::kMaxChannels);

        m_profile.collisionResponses[channelIndex] = response;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the collision response for a specific channel by name.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Collider::SetCollisionResponseToChannel(const char* pChannelName, const CollisionResponse response)
    {
        const CollisionChannel channel = Internal::CollisionChannelManager::GetOrAssignCollisionChannel(pChannelName);
        SetCollisionResponseToChannel(channel, response);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the CollisionResponse to all of the channels to match the 'response'.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Collider::SetCollisionResponseToAllChannels(const CollisionResponse response)
    {
        for(auto& myResponse : m_profile.collisionResponses)
        {
            myResponse = response;
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the response of this collider to a specific channel.
    //-----------------------------------------------------------------------------------------------------------------------------
    CollisionResponse Collider::GetResponseToChannel(const CollisionChannel channel) const
    {
        const size_t channelIndex = static_cast<size_t>(channel);
        assert(channelIndex < Internal::kMaxChannels);
        return m_profile.collisionResponses[channelIndex];
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Return how this collider will respond to another collider.
    ///		@param pOther : 
    ///		@returns : The CollisionResponse to the other collider's CollisionChannel.
    //-----------------------------------------------------------------------------------------------------------------------------
    CollisionResponse Collider::GetResponseToCollider(const Collider* pOther) const
    {
        const auto otherCollisionChannel = pOther->m_profile.myCollisionChannel;
        return GetResponseToChannel(otherCollisionChannel);
    }
}
