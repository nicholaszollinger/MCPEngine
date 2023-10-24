// ColliderComponent.cpp

#include "ColliderComponent.h"

#include "MCP/Core/Event/MessageManager.h"
#include "TransformComponent.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    ColliderComponent::ColliderComponent(Object* pObject, const bool collisionEnabled, const bool isStatic)
        : Component(pObject)
#if RENDER_COLLIDER_VISUALS
        , IRenderable(RenderLayer::kDebugOverlay)
#endif
        , m_pSystem(nullptr)
        , m_pCell(nullptr)
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
#if RENDER_COLLIDER_VISUALS
        if (m_collisionEnabled)
            m_pOwner->GetWorld()->RemoveRenderable(this);
#endif

        // If we were listening to the transform updated events, unregister.
        //if (!m_isStatic && m_collisionEnabled)
        //    m_pTransformComponent->m_onLocationUpdated.RemoveListener(this);

        // Remove ourselves from the collision system.
        if (m_collisionEnabled)
            m_pSystem->RemoveCollideable(this);

        // Remove ourselves from the physics update.
        if (!m_isStatic)
            m_pOwner->GetWorld()->RemovePhysicsUpdateable(this);

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
        auto* pWorld = m_pOwner->GetWorld();
        MCP_CHECK(pWorld);

        m_pTransformComponent = m_pOwner->GetComponent<TransformComponent>();
        if (!m_pTransformComponent)
        {
            MCP_ERROR("Collision", "Failed to initialize ColliderComponent! TransformComponent was nullptr!");
            return false;
        }

        m_lastLocation = m_pTransformComponent->GetLocation();
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
        // If we are an active, enabled collider, then we need to listen to the transform updates.
        //if (!m_isStatic && m_collisionEnabled)
        //{
        //    //m_pTransformComponent->m_onLocationUpdated.AddListener(this, [this](const Vec2 pos){ this->TestCollisionNow(pos);});
        //}

        // Add ourselves to the CollisionSystem
        if (m_collisionEnabled)
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
            m_velocity = m_pTransformComponent->GetLocation() - m_lastLocation;

            // If the x or y value is approximately zero, clamp it to zero.
            if (CheckEqualFloats(m_velocity.x, 0.f))
                m_velocity.x = 0;

            if (CheckEqualFloats(m_velocity.y, 0.f))
                m_velocity.y = 0;
            
            // Update our last location.
            m_lastLocation = m_pTransformComponent->GetLocation();
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

        // If collision is enabled, we need to update our transform listening.
        if (m_collisionEnabled)
        {
            // If we were enabled, then we need to update our status in the Collision system.
            m_pSystem->SetCollideableStatic(this, isStatic);

            //// If we are becoming static, then we need to stop listening to the transform updates.
            //if (isStatic)
            //{
            //    //m_pTransformComponent->m_onLocationUpdated.AddListener(this, [this](const Vec2 pos){ this->TestCollisionNow(pos);});
            //}

            // Otherwise, we are becoming active and need to start listening.
            //else
            //{
            //    //m_pTransformComponent->m_onLocationUpdated.RemoveListener(this);
            //}
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

        // If we aren't a static collider, then we need to update if we
        // are listening to the Transform's updates.
        //if (!m_isStatic)
        //{
            // If we are enabling the collision, then we need to
            //  - Add this Component to the collision grid.
            //  - Stop listening to Transform updates. Our collision will be updated by being active.
        if (isEnabled)
        {
#if RENDER_COLLIDER_VISUALS
            m_pOwner->GetWorld()->AddRenderable(this);
#endif

            m_pSystem->AddCollideable(this);
            //m_pTransformComponent->m_onLocationUpdated.RemoveListener(this);
        }
        
        // Otherwise, we to remove this component from the collision tree and start listening
        // to transform updates.
        else
        {
#if RENDER_COLLIDER_VISUALS
            m_pOwner->GetWorld()->RemoveRenderable(this);
#endif

            //MCP_LOG("Collision", "Removing Collideable...");
            m_pSystem->RemoveCollideable(this);
            //m_pTransformComponent->m_onLocationUpdated.AddListener(this, [this](const Vec2 pos){ this->TestCollisionNow(pos);});
        }
        //}

        m_collisionEnabled = isEnabled;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : If the Collider's data has changed like its size or enabled status, this will update the estimation rect. 
    //-----------------------------------------------------------------------------------------------------------------------------
    void ColliderComponent::ColliderCollisionChanged(const Collider::ColliderNameId id)
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
        result.SetPosition(result.GetPosition() + m_pTransformComponent->GetLocation());
        return result;
    }
    
    bool ColliderComponent::AddFromData(const XMLElement component, Object* pOwner)
    {
        // CollisionEnabled
        bool isEnabled = component.GetAttributeValue<bool>("collisionEnabled", true);

        // IsStatic
        bool isStatic = component.GetAttributeValue<bool>("isStatic");

        auto* pNewComponent = pOwner->AddComponent<ColliderComponent>(isEnabled, isStatic);
        // Add the component to the Object
        if (!pNewComponent)
        {
            MCP_ERROR("Collision", "Failed to add ColliderComponent from data!");
            return false;
        }

        // Get each collider for this component.
        auto colliderElement = component.GetChildElement();

        while (colliderElement.IsValid())
        {
            // Create the Collider from data, using a factory
            if (!ColliderFactory::AddNewFromData(colliderElement.GetName(), colliderElement, pNewComponent))
            {
                MCP_ERROR("Collision", "Failed to add Collider to new ColliderComponent from data!");
                return false;
            }

            // Get the next collider.
            colliderElement = colliderElement.GetSiblingElement();
        }

        return true;
    }

#if RENDER_COLLIDER_VISUALS
    void ColliderComponent::Render() const
    {
        if (!m_collisionEnabled)
            return;

        // I need to adjust the rect for rendering.
        //DrawRect(GetEstimationRect(), Color::White());

        for (auto& [colliderId, pCollider] : m_colliders)
        {
            pCollider->Render();
        }
    }
#endif
}