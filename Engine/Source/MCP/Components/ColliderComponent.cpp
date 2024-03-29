// ColliderComponent.cpp

#include "ColliderComponent.h"

#include "MCP/Core/Event/MessageManager.h"
#include "TransformComponent.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    ColliderComponent::ColliderComponent(const bool collisionEnabled, const bool isStatic)
        : Component(true)
#if RENDER_COLLIDER_VISUALS
        , IRenderable(RenderLayer::kDebugOverlay)
#endif
        , m_pSystem(nullptr)
        , m_pReferenceCell(nullptr)
        , m_pTransformComponent(nullptr)
        , m_myRelativeEstimationRect{}
        , m_activeColliderCount(0)
        , m_isStatic(isStatic)
        , m_collisionEnabled(collisionEnabled)
    {
        //
    }

    ColliderComponent::~ColliderComponent()
    {
        // If this Component was active,
        if (IsActive())
        {
            if (m_collisionEnabled)
            {
#if RENDER_COLLIDER_VISUALS
                GetOwner()->GetWorld()->RemoveRenderable(this);
#endif

                // Remove ourselves from the collision system.
                m_pSystem->RemoveCollideable(this);

                // Remove ourselves from the physics update.
                if (!m_isStatic)
                    GetOwner()->GetWorld()->RemovePhysicsUpdateable(this);
            }
        }

        // Delete all of our colliders
        for (auto&[ colliderId, pCollider] : m_colliders)
        {
            BLEACH_DELETE(pCollider);
            pCollider = nullptr;
        }
    }

    bool ColliderComponent::Init()
    {
        // Add this component to the list of renderables in the scene.
        auto* pWorld = GetOwner()->GetWorld();
        MCP_CHECK(pWorld);

        m_pTransformComponent = GetOwner()->GetComponent<TransformComponent>();
        if (!m_pTransformComponent)
        {
            MCP_ERROR("Collision", "Failed to initialize ColliderComponent! TransformComponent was nullptr!");
            return false;
        }

        m_lastLocation = m_pTransformComponent->GetPosition();
        UpdateEstimationRect();

        // Need to register with the collision system.
        m_pSystem = pWorld->GetCollisionSystem();

        if (!m_isStatic)
            pWorld->AddPhysicsUpdateable(this);

#if RENDER_COLLIDER_VISUALS
        if (m_collisionEnabled)
            pWorld->AddRenderable(this);
#endif

        return true;
    }

    bool ColliderComponent::PostLoadInit()
    {
        // Add ourselves to the CollisionSystem
        if (CollisionEnabled())
            m_pSystem->AddCollideable(this);

        return true;
    }

    void ColliderComponent::OnDestroy()
    {
        m_collisionEnabled = false;
    }

    void ColliderComponent::Update([[maybe_unused]] const float deltaTime)
    {
        // If we are an 'active' collider, calculate our velocity
        if (!m_isStatic)
        {
            // Calculate our velocity.
            m_velocity = m_pTransformComponent->GetPosition() - m_lastLocation;

            // If the x or y value is approximately zero, clamp it to zero.
            if (CheckEqualFloats(m_velocity.x, 0.f))
                m_velocity.x = 0;

            if (CheckEqualFloats(m_velocity.y, 0.f))
                m_velocity.y = 0;
            
            // Update our last location.
            m_lastLocation = m_pTransformComponent->GetPosition();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set whether this collider component is 'static' or not. Static means that this object won't be moving around
    ///         so we don't need to bother listening to transform updates.
    ///		@param isStatic : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void ColliderComponent::SetIsStatic(const bool isStatic)
    {
        // If we aren't changing our state, then just return.
        if (isStatic == m_isStatic)
            return;

        // If collision is enabled
        if (CollisionEnabled())
        {
            if (isStatic)
            {
                m_pSystem->SetCollideableStatic(this);
                GetOwner()->GetWorld()->RemovePhysicsUpdateable(this);
            }

            else
            {
                m_pSystem->SetCollideableActive(this);
                // If we were enabled, then we need to update our status in the Collision system.
                GetOwner()->GetWorld()->AddPhysicsUpdateable(this);
            }
        }

        m_isStatic = isStatic;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set whether we listen to collisions or not. Note: This does *not* effect the individual colliders. Their settings
    ///             won't change. So if you turned off collisions for this component, then turned them back on, if some of those
    ///             colliders were inactive, they will stay inactive.
    //-----------------------------------------------------------------------------------------------------------------------------
    void ColliderComponent::SetCollisionEnabled(const bool isEnabled)
    {
        // If our state isn't changing, return
        if (isEnabled == m_collisionEnabled)
            return;

        m_collisionEnabled = isEnabled;

        // If this Component is active, then we need to update our membership in the Collision System.
        if (IsActive())
        {
            if (m_collisionEnabled)
            {
#if RENDER_COLLIDER_VISUALS
                GetOwner()->GetWorld()->AddRenderable(this);
#endif

                m_pSystem->AddCollideable(this);
                //m_pTransformComponent->m_onLocationUpdated.RemoveListener(this);
            }
            
            // Otherwise, we to remove this component from the collision tree
            else
            {
#if RENDER_COLLIDER_VISUALS
                GetOwner()->GetWorld()->RemoveRenderable(this);
#endif

                //MCP_LOG("Collision", "Removing Collideable...");
                m_pSystem->RemoveCollideable(this);
                //m_pTransformComponent->m_onLocationUpdated.AddListener(this, [this](const Vec2 pos){ this->TestCollisionNow(pos);});
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Return whether this collider is being checked for collision against other colliders.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool ColliderComponent::CollisionEnabled() const
    {
        if (!IsActive())
        {
            return false;
        }

        return m_collisionEnabled;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : If the Collider's data has changed like its size or enabled status, this will update the estimation rect. 
    //-----------------------------------------------------------------------------------------------------------------------------
    void ColliderComponent::ColliderCollisionChanged([[maybe_unused]] const Collider::ColliderNameId id)
    {
        // If we don't have a collider with that id or the state is not changing, return.
        MCP_CHECK(m_colliders.find(id)->second);

        UpdateEstimationRect();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : This function is used for when a static collider (one that does not move) is teleported in some way to a
    ///         a new location.
    ///		@param newPosition : The new position of the component.
    //-----------------------------------------------------------------------------------------------------------------------------
    void ColliderComponent::TestCollisionNow([[maybe_unused]] const Vec2 newPosition)
    {
        m_lastLocation = newPosition;
        m_pSystem->CheckCollision(this);
    }

    void ColliderComponent::SetColliderEnabled(const Collider::ColliderNameId id, const bool isEnabled)
    {
        const auto result = m_colliders.find(id);

        // If we don't have a collider with that id or the state is not changing, return.
        if (result == m_colliders.end() || result->second->CollisionIsEnabled() == isEnabled)
            return;

        auto* pCollider = result->second;
        pCollider->SetCollisionEnabled(isEnabled);

        // Update the estimation rect.
        UpdateEstimationRect();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Add a collider to the Component.
    ///		@param pCollider : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void ColliderComponent::AddCollider(Collider* pCollider)
    {
        const Collider::ColliderNameId nameId = pCollider->GetNameId();
        if (const auto result = m_colliders.find(nameId); result != m_colliders.end())
        {
            MCP_WARN("Collision", "Tried to add a Collider that already exists with that name! You may have a memory leak depending on how you are trying to add this!");
            return;
        }

        // Add the collider to our container and set their owner.
        m_colliders.emplace(nameId, pCollider);
        pCollider->SetOwner(this);
        pCollider->SetSystem(m_pSystem);

        // If we are adding an active collider, then we need to recalculate our estimation.
        if (pCollider->CollisionIsEnabled())
            UpdateEstimationRect();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Remove a collider from the component, destroying it.
    ///		@param pColliderName : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void ColliderComponent::RemoveCollider(const char* pColliderName)
    {
        const Collider::ColliderNameId nameId = HashString32(pColliderName);
        const auto result = m_colliders.find(nameId);
        if (result == m_colliders.end())
        {
            MCP_WARN("Collision", "Tried to remove a Collider named '", pColliderName, "' that doesn't exist on the ColliderComponent");
            return;
        }

        // If the collider's collision was enabled,
        const bool wasEnabled = result->second->CollisionIsEnabled();
        if (wasEnabled && result->second->IsOverlapping())
        {
            m_pSystem->RemoveOverlappingCollider(result->second);   
        }

        // Delete the collider.
        BLEACH_DELETE(result->second);
        result->second = nullptr;

        // Remove the collider:
        m_colliders.erase(result);

        // If it was enabled, then we need to update our estimation rect.
        if (wasEnabled)
            UpdateEstimationRect();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		This is called whenever there is a change to our colliders, whether they're active state changes,
    //      whether we have added or removed colliders, etc.
    //
    ///		@brief : Calculate the smallest rect possible that encompasses all of our colliders.
    //-----------------------------------------------------------------------------------------------------------------------------
    void ColliderComponent::UpdateEstimationRect()
    {
        //const Vec2 location = m_pTransformComponent->GetLocation();
        m_myRelativeEstimationRect = {0.f, 0.f, 0.f, 0.f};
        m_activeColliderCount = 0;

        if(m_colliders.empty())
        {
            return;
        }

        // If we have just one collider, then our estimation will equal the
        // estimated rect of that collider.
        if (m_colliders.size() == 1)
        {
            auto& [id, pComponent] = *m_colliders.begin();
            if (pComponent->CollisionIsEnabled())
            {
                ++m_activeColliderCount;
                m_myRelativeEstimationRect = pComponent->GetEstimateRectRelative();
            }

            return;
        }

        for (auto& [id, pCollider] : m_colliders)
        {
            if (!pCollider->CollisionIsEnabled())
                continue;

            ++m_activeColliderCount;
            const RectF colliderEstimation = pCollider->GetEstimateRectRelative();

            // If the left corner is to the left of our rect, increase our width by the difference and
            // move our point over to to match the left most distance.
            if (colliderEstimation.x < m_myRelativeEstimationRect.x)
            {
                m_myRelativeEstimationRect.width += m_myRelativeEstimationRect.x - colliderEstimation.x;
                m_myRelativeEstimationRect.x = colliderEstimation.x;
            }
            
            // If the right corner is to the right of our estimation increase our width by the difference
            if (colliderEstimation.x + colliderEstimation.width > m_myRelativeEstimationRect.x + m_myRelativeEstimationRect.width)
            {
                m_myRelativeEstimationRect.width += (colliderEstimation.x + colliderEstimation.width) - (m_myRelativeEstimationRect.x + m_myRelativeEstimationRect.width);
            }

            // If the top of the collider is higher than our top, increase our height by the difference
            // and move our point up to match it.
            if (colliderEstimation.y < m_myRelativeEstimationRect.y)
            {
                m_myRelativeEstimationRect.height += m_myRelativeEstimationRect.y - colliderEstimation.y;
                m_myRelativeEstimationRect.y = colliderEstimation.y;
            }

            // If the bottom of the collider is lower than our bottom, increase our height by the difference.
            if (colliderEstimation.y + colliderEstimation.height > m_myRelativeEstimationRect.y + m_myRelativeEstimationRect.height)
            {
                m_myRelativeEstimationRect.height += (colliderEstimation.y + colliderEstimation.height) - (m_myRelativeEstimationRect.y + m_myRelativeEstimationRect.height);
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the bounding box that holds all of our child colliders, in world space.
    //-----------------------------------------------------------------------------------------------------------------------------
    RectF ColliderComponent::GetEstimationRect() const
    {
        RectF result = m_myRelativeEstimationRect;
        result.SetPosition(result.GetPosition() + m_pTransformComponent->GetPosition());
        return result;
    }

    ColliderComponent* ColliderComponent::AddFromData(const XMLElement element)
    {
        // CollisionEnabled
        bool isEnabled = element.GetAttributeValue<bool>("collisionEnabled", true);

        // IsStatic
        bool isStatic = element.GetAttributeValue<bool>("isStatic");

        auto* pNewComponent = BLEACH_NEW(ColliderComponent(isEnabled, isStatic));
        // Add the component to the Object
        if (!pNewComponent)
        {
            MCP_ERROR("Collision", "Failed to add ColliderComponent from data!");
            BLEACH_DELETE(pNewComponent);
            return nullptr;
        }

        // Get each collider for this component.
        auto colliderElement = element.GetChildElement();

        while (colliderElement.IsValid())
        {
            // Create the Collider from data, using a factory
            if (!ColliderFactory::AddNewFromData(colliderElement.GetName(), colliderElement, pNewComponent))
            {
                MCP_ERROR("Collision", "Failed to add Collider to new ColliderComponent from data!");
                BLEACH_DELETE(pNewComponent);
                return nullptr;
            }

            // Get the next collider.
            colliderElement = colliderElement.GetSiblingElement();
        }

        return pNewComponent;
    }

    void ColliderComponent::OnActive()
    {
        if (m_collisionEnabled)
        {
            m_pSystem->AddCollideable(this);
        }

        // If we are not static, then we need to add ourselves to the physics update.
        if (!m_isStatic)
        {
            m_velocity = Vec2::ZeroVector();
            m_lastLocation = m_pTransformComponent->GetPosition();
            GetOwner()->GetWorld()->AddPhysicsUpdateable(this);
        }

        // Notify our colliders of the active change:
        for (auto&[colliderId, pCollider] : m_colliders)
        {
            pCollider->OnComponentActiveChanged(true);
        }

#if RENDER_COLLIDER_VISUALS
            GetOwner()->GetWorld()->AddRenderable(this);
#endif
    }

    void ColliderComponent::OnInactive()
    {
        if (m_collisionEnabled)
            m_pSystem->RemoveCollideable(this);

        // If we are not static, then we need to remove ourselves from the physics update.
        if (!m_isStatic)
            GetOwner()->GetWorld()->RemovePhysicsUpdateable(this);

        // Notify our colliders of the active change:
        for (auto&[colliderId, pCollider] : m_colliders)
        {
            pCollider->OnComponentActiveChanged(false);
        }

#if RENDER_COLLIDER_VISUALS
        GetOwner()->GetWorld()->RemoveRenderable(this);
#endif
    }

#if RENDER_COLLIDER_VISUALS
    void ColliderComponent::Render() const
    {
        if (!CollisionEnabled())
            return;

        for (auto& [colliderId, pCollider] : m_colliders)
        {
            pCollider->Render();
        }
    }
#endif
}