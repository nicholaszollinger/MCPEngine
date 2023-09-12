// ColliderComponent.cpp

#include "ColliderComponent.h"

#include "MCP/Core/Event/MessageManager.h"
#include "TransformComponent.h"
#include "MCP/Scene/Scene.h"

#ifdef MCP_DATA_PARSER_TINYXML2
#include "Platform/TinyXML2/tinyxml2.h"
#else
#error "ImageComponent does not have support a the currently defined Data Parser!"
#endif

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
        , m_activeColliderCount(0)
        , m_isStatic(isStatic)
        , m_collisionEnabled(collisionEnabled)
    {
        //
    }

    ColliderComponent::~ColliderComponent()
    {
#if RENDER_COLLIDER_VISUALS
        m_pOwner->GetScene()->RemoveRenderable(this);
#endif

        // If we were listening to the transform updated events, unregister.
        if (!m_isStatic && m_collisionEnabled)
            m_pTransformComponent->m_onLocationUpdated.RemoveListener(this);

        // Remove ourselves from the collision system.
        m_pSystem->RemoveCollideable(this);

        // Remove ourselves from the physics update.
        m_pOwner->GetScene()->RemovePhysicsUpdateable(this);

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
        auto* pScene = m_pOwner->GetScene();
        assert(pScene);

        m_pTransformComponent = m_pOwner->GetComponent<TransformComponent>();
        if (!m_pTransformComponent)
        {
            LogError("Failed to initialize ColliderComponent! TransformComponent was nullptr!");
            return false;
        }

        m_lastLocation = m_pTransformComponent->GetLocation();
        UpdateEstimationRect();

        // Need to register with the collision system.
        m_pSystem = pScene->GetCollisionSystem();

        pScene->AddPhysicsUpdateable(this);

#if RENDER_COLLIDER_VISUALS
        pScene->AddRenderable(this);
#endif

        return true;
    }

    bool ColliderComponent::PostLoadInit()
    {
        // If we are an active, enabled collider, then we need to listen to the transform updates.
        if (!m_isStatic && m_collisionEnabled)
        {
            m_pTransformComponent->m_onLocationUpdated.AddListener(this, [this](const Vec2 pos){ this->TestCollisionNow(pos);});
        }

        // Add ourselves to the CollisionSystem
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
            // If we are becoming static, then we need to stop listening to the transform updates.
            if (isStatic)
            {
                m_pTransformComponent->m_onLocationUpdated.AddListener(this, [this](const Vec2 pos){ this->TestCollisionNow(pos);});
            }

            // Otherwise, we are becoming active and need to start listening.
            else
            {
                m_pTransformComponent->m_onLocationUpdated.RemoveListener(this);
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

        // If we aren't a static collider, then we need to update if we
        // are listening to the Transform's updates.
        if (!m_isStatic)
        {
            // If we are enabling the collision, then we need to
            //  - Add this Component to the collision grid.
            //  - Stop listening to Transform updates. Our collision will be updated by being active.
            if (isEnabled)
            {
                m_pSystem->AddCollideable(this);
                m_pTransformComponent->m_onLocationUpdated.RemoveListener(this);
            }
            
            // Otherwise, we to remove this component from the collision tree and start listening
            // to transform updates.
            else
            {
                m_pSystem->RemoveCollideable(this);
                m_pTransformComponent->m_onLocationUpdated.AddListener(this, [this](const Vec2 pos){ this->TestCollisionNow(pos);});
            }
        }

        m_collisionEnabled = isEnabled;
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
            LogWarning("Tried to add a Collider that already exists with that name! You may have a memory leak depending on how you are trying to add this!");
            return;
        }

        // Add the collider to our container and set their owner.
        m_colliders.emplace(nameId, pCollider);
        pCollider->SetOwner(this);

        // If we are adding an active collider, then we need to recalculate our estimation.
        if (pCollider->CollisionIsEnabled())
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
                m_myRelativeEstimationRect = pComponent->GetEstimateRectRelative();
            }

            return;
        }

        for (auto& [id, pCollider] : m_colliders)
        {
            if (!pCollider->CollisionIsEnabled())
                continue;

            const RectF colliderEstimation = pCollider->GetEstimateRectRelative();

            // If the left corner is to the left of our rect, increase our width by the difference and
            // move our point over to to match the left most distance.
            if (colliderEstimation.position.x < m_myRelativeEstimationRect.position.x)
            {
                m_myRelativeEstimationRect.width += colliderEstimation.position.x - m_myRelativeEstimationRect.position.x;
                m_myRelativeEstimationRect.position.x = colliderEstimation.position.x;
            }
            
            // If the right corner is to the right of our estimation increase our width by the difference
            if (colliderEstimation.position.x + colliderEstimation.width > m_myRelativeEstimationRect.position.x + m_myRelativeEstimationRect.width)
            {
                m_myRelativeEstimationRect.width += (colliderEstimation.position.x + colliderEstimation.width) - (m_myRelativeEstimationRect.position.x + m_myRelativeEstimationRect.width);
            }

            // If the top of the collider is higher than our top, increase our height by the difference
            // and move our point up to match it.
            if (colliderEstimation.position.y < m_myRelativeEstimationRect.position.y)
            {
                m_myRelativeEstimationRect.height += m_myRelativeEstimationRect.position.y - colliderEstimation.position.y;
                m_myRelativeEstimationRect.position.y = colliderEstimation.position.y;
            }

            // If the bottom of the collider is lower than our bottom, increase our height by the difference.
            if (colliderEstimation.position.y + colliderEstimation.height > m_myRelativeEstimationRect.position.y + m_myRelativeEstimationRect.height)
            {
                m_myRelativeEstimationRect.height += colliderEstimation.position.y + colliderEstimation.height - m_myRelativeEstimationRect.position.y + m_myRelativeEstimationRect.height;
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
        result.position += m_pTransformComponent->GetLocation();
        return result;
    }

#ifdef MCP_DATA_PARSER_TINYXML2
    bool ColliderComponent::AddFromData(const void* pFileData, Object* pOwner)
    {
        auto* pColliderComponent = static_cast<const tinyxml2::XMLElement*>(pFileData);

        // CollisionEnabled
        bool isEnabled = pColliderComponent->BoolAttribute("collisionEnabled");

        // IsStatic
        bool isStatic = pColliderComponent->BoolAttribute("isStatic");

        ColliderComponent* pNewComponent = pOwner->AddComponent<ColliderComponent>(isEnabled, isStatic);
        // Add the component to the Object
        if (!pNewComponent)
        {
            LogError("Failed to add ColliderComponent from data!");
            return false;
        }

        // Get each collider for this component.
        auto* pCollider = pColliderComponent->FirstChildElement();

        while (pCollider)
        {
            // Create the Collider from data, using a factory
            if (!ColliderFactory::AddNewFromData(pCollider->Value(), pCollider, pNewComponent))
            {
                LogError("Failed to add Collider to new ColliderComponent from data!");
                return false;
            }

            // Get the next collider.
            pCollider = pCollider->NextSiblingElement();
        }

        return true;
    }
#else
    bool ImageComponent::AddFromData(const void*, Object*)
    {
        return false;
    }
#endif

#if RENDER_COLLIDER_VISUALS
    void ColliderComponent::Render() const
    {
        for (auto& [colliderId, pCollider] : m_colliders)
        {
            pCollider->Render();
        }
    }
#endif
}
