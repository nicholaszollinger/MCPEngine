#pragma once
// Collider.h

#include <unordered_set>

#include "ColliderFactory.h"

#include "CollisionProfile.h"
#include "MCP/Core/Event/MulticastDelegate.h"
#include "Utility/Types/Rect.h"

#ifdef _DEBUG
//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      Rendering of colliders is only available in Debug mode.
//		
///		@brief : Macro toggle to render the outlines of the colliders in the game.
//-----------------------------------------------------------------------------------------------------------------------------
#define RENDER_COLLIDER_VISUALS 1

#if RENDER_COLLIDER_VISUALS
    #include "MCP/Graphics/Graphics.h"
#endif

#else
    #define RENDER_COLLIDER_VISUALS 0
#endif

namespace mcp
{
    class CollisionSystem;
    class Collider;
    class Object;

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      This works essentially the same as our ComponentTypeId generation.
    //		
    ///		@brief : Registers the 'Create' function for the Collider type to successfully create a Collider from data. This also
    ///         generates an interface for getting the type id of the Collider -> static GetStaticTypeId() and a virtual GetTypeId().
    ///		@param ColliderTypeName : Name of the ColliderType. We want a type name, not a string!
    //-----------------------------------------------------------------------------------------------------------------------------
#define MCP_DEFINE_COLLIDER_TYPE_ID(ColliderTypeName)                                                                                                   \
private:                                                                                                                                                \
    static const inline mcp::ColliderTypeId kColliderTypeId = mcp::ColliderFactory::RegisterColliderType<ColliderTypeName>(#ColliderTypeName);          \
public:                                                                                                                                                 \
    static mcp::ColliderTypeId GetStaticTypeId() { return kColliderTypeId; }                                                                            \
    virtual mcp::ColliderTypeId GetTypeId() const override { return kColliderTypeId; }                                                                  \
private:

    // Delegate type for when two colliders hit.
    using OnHit = MulticastDelegate<Collider*, Object*>;
    // Delegate type for when two colliders overlap.
    using OnOverlap = MulticastDelegate<Collider*, Object*>;

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Base class for the Colliders in the Game.
    //-----------------------------------------------------------------------------------------------------------------------------
    class Collider
    {
        friend class CollisionSystem;
        friend class ColliderComponent;

    public:
        using ColliderNameId = uint32_t;

    public:
        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //		
        ///		@brief : This delegate is called when this collider hits another collider. However, a hit is only registered if the two
        ///             colliders responses to the each other's channel is 'Block'.
        ///             \n Example:
        ///                 - A Player object is in the 'Player' channel and blocks all other channels.
        ///                 - A Wall object is in the 'Default' channel and blocks all other channels.
        ///                 - Since both of the objects have a 'block' response to each other's channel, when they collide
        ///                   this hit delegate will be called.
        ///                 - If either collider had their response set to 'Overlap', then the OnBeginOverlap delegate will be called, and the
        ///                   two objects would 'phase through' each other.
        ///                 - If either collider had their response set to 'Ignore', then no collision would occur.
        //-----------------------------------------------------------------------------------------------------------------------------
        OnHit m_onHit;

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //		
        ///		@brief : This delegate is called when this collider 'enters' another collider. For this to happen, one or both of the colliders
        ///           need have their response set to 'Overlap'.
        ///             \n Example:
        ///                 - A Player object is in the 'Player' channel and blocks all other channels.
        ///                 - A Coin object is in the 'Item' channel and overlaps all other channels.
        ///                 - Since the Coin object's response to the Player channel is Overlap, when they collide for the first time, this
        ///                   delegate will be called, and the objects would phase through each other.
        ///                 - If both colliders had their response set to 'Block', then the OnHit delegate will be called, and the
        ///                   two objects would run into each other.
        ///                 - If either collider had their response set to 'Ignore', then no collision would occur.
        //-----------------------------------------------------------------------------------------------------------------------------
        OnOverlap m_onBeginOverlap;

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //		
        ///		@brief : This delegate is called every frame that this collider and the other collider are still overlapping.
        //-----------------------------------------------------------------------------------------------------------------------------
        OnOverlap m_onOverlapUpdate;

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //		
        ///		@brief : This delegate is called when this collider stops intersecting another collider that had originally sent the
        ///         OnBeginOverlap call. If the two colliders happen to be overlapping but they don't have an interest in each other's channel
        ///         this will NOT be called.
        ///             \n Example:
        ///                 - A Player object is in the 'Player' channel and blocks all other channels.
        ///                 - A Coin object is in the 'Item' channel and overlaps all other channels.
        ///                 - The Player had begun walking over coin which had called the OnBeginOverlap delegate.
        ///                 - When the Player finishes walking over the coin, this delegate will be called.
        //-----------------------------------------------------------------------------------------------------------------------------
        OnOverlap m_onExitOverlap;

    protected:
        Vec2 m_relativePosition;                    // Offset from the Transform component.
        CollisionSystem* m_pSystem;                 // Reference to the collision system. NOT CURRENTLY USED.
        ColliderComponent* m_pOwner;                // The Collider Component that owns this collider.
        ColliderNameId m_colliderName;                          // NameId to identify this collider.

    private:
        std::unordered_set<Collider*> m_overlappingColliders;   // The colliders that are currently overlapping this collider.
        CollisionProfile m_profile;                             // Defines how this Collider interacts with other collision channels.
        bool m_isEnabled;

    public:
        Collider(const char* name, const bool isEnabled, const Vec2 position);
        virtual ~Collider() = default;

        void SetCollisionEnabled(const bool isEnabled);
        void SetOwner(ColliderComponent* pComponent) { m_pOwner = pComponent; }
        void SetSystem(CollisionSystem* pSystem) { m_pSystem = pSystem; }

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Get the smallest rect possible that encompasses the Collider's shape, in relative space. The position 
        ///        of the rect will represent the top left corner.
        //-----------------------------------------------------------------------------------------------------------------------------
        [[nodiscard]] virtual RectF GetEstimateRectRelative() const = 0;
        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Get the smallest rect possible that encompasses the Collider's shape, in world space. The position 
        ///        of the rect will represent the top left corner.
        //-----------------------------------------------------------------------------------------------------------------------------
        [[nodiscard]] virtual RectF GetEstimateRectWorld() const = 0;
        [[nodiscard]] virtual ColliderTypeId GetTypeId() const = 0;
        [[nodiscard]] virtual ColliderNameId GetNameId() const { return m_colliderName; }
        [[nodiscard]] Vec2 GetWorldCenter() const;
        [[nodiscard]] bool CollisionIsEnabled() const { return m_isEnabled; }
        [[nodiscard]] ColliderComponent* GetOwner() const { return m_pOwner; }
        [[nodiscard]] bool IsOverlapping() const { return !m_overlappingColliders.empty(); }

        //--------------------------------------------------------------------
        //  Collision Profile Interface
        //--------------------------------------------------------------------
        void SetMyCollisionChannel(const CollisionChannel channel);
        void SetMyCollisionChannel(const char* pChannelName);
        void SetCollisionResponseToChannel(const CollisionChannel channel, const CollisionResponse response);
        void SetCollisionResponseToChannel(const char* pChannelName, const CollisionResponse response);
        void SetCollisionResponseToAllChannels(const CollisionResponse response);
        [[nodiscard]] CollisionResponse GetResponseToChannel(const CollisionChannel channel) const;
        [[nodiscard]] CollisionResponse GetResponseToCollider(const Collider* pOther) const;
        [[nodiscard]] CollisionChannel GetMyCollisionChannel() const { return m_profile.myCollisionChannel;} 

    private:
        void OnComponentActiveChanged(const bool isActive);

        //--------------------------------------------------------------------
        //  Debug Rendering
        //--------------------------------------------------------------------
#if RENDER_COLLIDER_VISUALS
    protected:
        static constexpr Color kColliderDebugColor{255,0,180};
    public:
        virtual void Render() const = 0;
#endif
    };
}