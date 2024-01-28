#pragma once
// ColliderComponent.h

#include "Component.h"
#include "MCP/Collision/Collider.h"
#include "MCP/Collision/CollisionSystem.h"
#include "MCP/Components/TransformComponent.h"
#include "MCP/Scene/IUpdateable.h"

#if _DEBUG && RENDER_COLLIDER_VISUALS
    #include "MCP/Scene/IRenderable.h"
    #include "Utility/Types/Color.h"
#endif

namespace mcp
{
    class ColliderComponent final : public Component , public IUpdateable
#if RENDER_COLLIDER_VISUALS 
        , public IRenderable    // Only inherit from this class defined.
#endif
    {
        MCP_DEFINE_COMPONENT_ID(ColliderComponent)

    private:
        friend class CollisionSystem;

        using ColliderContainer = std::unordered_map<Collider::ColliderNameId, Collider*>;

        ColliderContainer m_colliders;              // Colliders that we own.
        CollisionSystem* m_pSystem;                 // CollisionSystem reference.
        std::vector<void*> m_cells;
        void* m_pReferenceCell;                     // Highest cell in the collision tree that completely encapsulated our rect.
        TransformComponent* m_pTransformComponent;  // The transform component we are attached to.
        RectF m_myRelativeEstimationRect;           // This is a collider that encompasses all of the child colliders. Used as a quick filter.
        Vec2 m_lastLocation;                        // The old location we have before any move.
        Vec2 m_velocity;                            // The velocity that this collideable is currently moving at.
        size_t m_activeColliderCount;               // The number of active colliders that we own.
        bool m_isStatic;                            // Whether this is a static collider or not.
        bool m_collisionEnabled;                    // Whether the collision for this component is enabled or not.

    public:
        ColliderComponent(const bool collisionEnabled, const bool isStatic);
        virtual ~ColliderComponent() override;

    public:
        // Component Interface
        virtual bool Init() override;
        virtual bool PostLoadInit() override;
        virtual void OnDestroy() override;
        virtual void Update(const float deltaTime) override;

        // Collider Component Behavior
        void SetIsStatic(const bool isStatic);
        void SetCollisionEnabled(const bool isEnabled);
        void ColliderCollisionChanged(const Collider::ColliderNameId id);

        // Collider access
        template<typename ColliderType> ColliderType* GetCollider(const char* pColliderName);
        void AddCollider(Collider* pCollider);
        void SetColliderEnabled(const Collider::ColliderNameId id, const bool isEnabled);
        void RemoveCollider(const char* pColliderName);

        void SetVelocity(const Vec2& velocity) { m_velocity = velocity; }
        [[nodiscard]] RectF GetEstimationRect() const;
        [[nodiscard]] TransformComponent* GetTransformComponent() const { return m_pTransformComponent; }
        [[nodiscard]] Vec2 GetVelocity() const { return m_velocity; }
        [[nodiscard]] bool CollisionEnabled() const;
        [[nodiscard]] size_t GetActiveColliderCount() const { return m_activeColliderCount; }
        
        static ColliderComponent* AddFromData(const XMLElement element);

    private:
        virtual void OnActive() override;
        virtual void OnInactive() override;
        void TestCollisionNow(const Vec2 newPosition);
        void UpdateEstimationRect();

//-----------------------------------------------------------------------------------------------------------------------------
//  DEBUG INTERFACE
//-----------------------------------------------------------------------------------------------------------------------------
#if RENDER_COLLIDER_VISUALS 
    protected:
        // Color for colliders to use to draw to the screen.
        static constexpr Color kColliderDebugColor = { 255,0,255,150 };
    public:
        virtual void Render() const override;
#endif
    };

    template <typename ColliderType>
    ColliderType* ColliderComponent::GetCollider(const char* pColliderName)
    {
        // Try to get our collider from our map,
        if (auto result = m_colliders.find(HashString32(pColliderName)); result != m_colliders.end())
        {
            // If the types don't match, report the issue and return nullptr.
            if (ColliderType::GetStaticTypeId() != result->second->GetTypeId())
            {
                MCP_WARN("Collision", "Failed to find Collider! Collider ", pColliderName, " did not match requested type!");
                return nullptr;
            }

            // Success!
            return static_cast<ColliderType*>(result->second);
        }

        MCP_WARN("Collision", "Failed to find Collider! No collider found with the name: ", pColliderName);
        // If we don't have it, return nullptr.
        return nullptr;
    }
}