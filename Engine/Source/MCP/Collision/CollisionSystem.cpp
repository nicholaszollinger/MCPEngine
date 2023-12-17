// CollisionSystem.cpp

#include "CollisionSystem.h"
#include "Box2DCollider.h"
#include "Collider.h"
#include "MCP/Components/ColliderComponent.h"
#include "MCP/Scene/Object.h"

namespace mcp
{
    CollisionSystem::CollisionSystem(const QuadtreeBehaviorData& data)
#if DEBUG_RENDER_COLLISION_TREE
        : IRenderable(RenderLayer::kDebugOverlay, -5)
        , m_pRoot(BLEACH_NEW(QuadtreeCell(nullptr)))
#else
        : m_pRoot(BLEACH_NEW(QuadtreeCell(nullptr)))
#endif
        , m_maxTreeDepth(data.maxDepth)
        , m_objectCountToTriggerDivide(data.maxObjectsInCell)
        , m_worldWidth(data.worldWidth)
        , m_worldHeight(data.worldHeight)
    {
        m_pRoot->dimensions = {0.f, 0.f, data.worldWidth, data.worldHeight};
    }

    CollisionSystem::~CollisionSystem()
    {
        ClearTree();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the behavior settings for the collision system.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::SetQuadtreeBehaviorData(const QuadtreeBehaviorData& data)
    {
        m_worldWidth = data.worldWidth;
        m_worldHeight = data.worldHeight;
        m_objectCountToTriggerDivide = data.maxObjectsInCell;
        m_maxTreeDepth = data.maxDepth;

        m_pRoot->dimensions = { data.worldXPos, data.worldYPos, data.worldWidth, data.worldHeight};
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Add a ColliderComponent to our tree to involve it in collision checks. Note, adding may cause a subdivision
    ///             the tree. The ColliderComponent's internal m_pCell value will be set to the Cell that it lands in.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::AddCollideable(ColliderComponent* pColliderComponent)
    {
        UpdateMembership(pColliderComponent);

        // If we are an active collider, add it to our list of active colliders.
        if (!pColliderComponent->m_isStatic)
            AddActiveCollider(pColliderComponent);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Remove a ColliderComponent from the tree.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::RemoveCollideable(ColliderComponent* pColliderComponent)
    {
        const auto rect = pColliderComponent->GetEstimationRect();
        RemoveDanglingCells(pColliderComponent, rect);

        // If we are not static, remove ourselves from the active collider array:
        if (!pColliderComponent->m_isStatic)
        {
            RemoveActiveCollider(pColliderComponent);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Go through and run collision checks for all of our active colliders.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::RunCollisions()
    {
        // Run collisions for all of our active Colliders
        for (auto* pColliderComponent : m_activeColliders)
        {
            CheckCollision(pColliderComponent);
        }

        UpdateOverlappingColliders();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : If the collider is in our tree, remove it from our actively updated colliders.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::SetCollideableStatic(ColliderComponent* pColliderComponent)
    {
        // If the collider Component was active, then we need to remove it.
        if (pColliderComponent->IsActive())
        {
            RemoveActiveCollider(pColliderComponent);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Adds the Collider Component to our array of colliders that we constantly check collisions for.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::SetCollideableActive(ColliderComponent* pColliderComponent)
    {
        // If there are no actual active colliders, don't add it to the array.
        if (pColliderComponent->GetActiveColliderCount() == 0 || !pColliderComponent->IsActive())
            return;

        AddActiveCollider(pColliderComponent);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Test the Collision of a ColliderComponent after a move.
    ///		@param  pColliderComponent : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::CheckCollision(ColliderComponent* pColliderComponent)
    {
        // We shouldn't be in the tree if our collision is disabled.
        MCP_CHECK(pColliderComponent->m_collisionEnabled);

        // This one should be handled earlier... When the the owner of the Collider moves.
        UpdateMembership(pColliderComponent);

        // If we are no longer in the bounds of the collision tree, return.
        if (pColliderComponent->m_cells.empty())
            return;

        // If we are still in the Tree, Run the collision.
        RunCollisionForColliderComponent(pColliderComponent);

        // Update the membership post-collision
        UpdateMembership(pColliderComponent);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      TODO: This function should be broken into smaller functions.
    //		
    ///		@brief : Takes in the cells that the Collider Component is in and Checks for collisions in each cell.
    ///		@param pColliderComponent : Component we are testing.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::RunCollisionForColliderComponent(ColliderComponent* pColliderComponent)
    {
        const RectF& estimationRect = pColliderComponent->GetEstimationRect();

        for (auto* pVoid : pColliderComponent->m_cells)
        {
            auto* pCell = static_cast<QuadtreeCell*>(pVoid);
            // If we have 1 or less components in this cell, then continue.
            if (pCell->m_colliderComponents.size() <= 1)
                continue;

            // For each other collider component, we need to see if we collide.
            for (auto& pComponent : pCell->m_colliderComponents)
            {
                // Don't check against itself.
                if (pComponent->GetOwner() == pColliderComponent->GetOwner())
                    continue;

                // Make sure this component is still valid for collision. Collisions may result in objects being destroyed,
                // colliders may be turned off, etc.
                if (pComponent->GetOwner()->IsQueuedForDeletion())
                    continue;

                // If the bounding boxes of the ColliderComponents don't intersect, then we can leave.
                if (!pComponent->GetEstimationRect().Intersects(estimationRect))
                    continue;

                // If they do intersect, then we need to check our individual colliders.
                //      NOTE: I am assuming *only* Box2DColliders which are *axis aligned*.
                for (auto& [name, pCollider] : pColliderComponent->m_colliders)
                {
                    // I shouldn't have to do this, I should be able to just grab the active colliders.
                    if (!pCollider->CollisionIsEnabled())
                            continue;

                    const RectF myEstimateRect = pCollider->GetEstimateRectWorld();

                    // For each active collider in pComponent
                    for (auto& [otherName, pOtherCollider] : pComponent->m_colliders)
                    {
                        // I shouldn't have to do this, I should be able to just grab the active colliders.
                        if (!pOtherCollider->CollisionIsEnabled())
                            continue;

                        const RectF otherEstimateRect = pOtherCollider->GetEstimateRectWorld();

                        // Get the responses for each collider.
                        const auto myResponse = pCollider->GetResponseToCollider(pOtherCollider);
                        const auto otherResponse = pOtherCollider->GetResponseToCollider(pCollider);

                        // Get the or'd together response to determine what to do.
                        const auto combinedResponse = myResponse | otherResponse;

                        // If either Collider is ignoring the other channel, then we don't need to worry about the collision.
                        if ((combinedResponse & CollisionResponse::kIgnore) == CollisionResponse::kIgnore)
                        {
                            continue;
                        }

                        // Calculate the overlap of the two colliders, as a rect.
                        const RectF intersectionRect = myEstimateRect.GetIntersectionAsRect(otherEstimateRect);

                        // If the intersectRect valid dimensions (a positive width and height), then we have an intersection.
                        if (intersectionRect.HasValidDimensions())
                        {
                            // If the combined response is to block, meaning that both colliders block each other, then
                            // we know that we need to perform a physics calculation and send 'OnHit' events.
                            if (combinedResponse == CollisionResponse::kBlock)
                            {
                                // Again, I am assuming that we don't have any rotation.
                                const bool significantFaceIsWidth = intersectionRect.width > intersectionRect.height;
                                const Vec2 significantFaceNormal = significantFaceIsWidth ? Vec2{1.f, 0.f} : Vec2{0.f, 1.f};

                                // Get how far the Collider should be moving along the face normal according to their velocity.
                                const Vec2 velocity = pColliderComponent->GetVelocity(); // Velocity is currently the just component's.
                                const float distanceOnFaceNormal = velocity.GetDotProduct(significantFaceNormal);
                                //mcp::Log("Velocity Projection onto significant face: %", distanceOnFaceNormal);

                                // If we are 'sliding' across the top face, meaning the width, then we need to move our vertical position back to in front of the
                                // collided object, and vice versa.
                                float distanceScalar = 1.f;
                                // If we are below or to the left of the the object we are hitting, then we need to be moving back.
                                if (significantFaceIsWidth && myEstimateRect.y < otherEstimateRect.y
                                    || !significantFaceIsWidth && myEstimateRect.x < otherEstimateRect.x)
                                {
                                    distanceScalar = -1.f;
                                }

                                const Vec2 deltaPos = significantFaceIsWidth ? Vec2{0, distanceScalar * intersectionRect.height} : Vec2 { distanceScalar * intersectionRect.width, 0.f};

                                pColliderComponent->GetTransformComponent()->AddToPosition(deltaPos);
                                pColliderComponent->SetVelocity(significantFaceNormal * distanceOnFaceNormal);

                                // Broadcast the events.
                                pCollider->m_onHit.Broadcast(pOtherCollider, pComponent->GetOwner());
                                pOtherCollider->m_onHit.Broadcast(pCollider, pColliderComponent->GetOwner());
                            }

                            // If either one of the responses were CollisionResponse::Overlap, then we don't need to affect the physics in
                            // any way, but we need to handle the overlap.
                            else
                            {
                                // If we don't have the other collider in our list of overlapping colliders,
                                if (auto result = pCollider->m_overlappingColliders.find(pOtherCollider); result == pCollider->m_overlappingColliders.end())
                                {
                                    // Broadcast the begin overlap event.
                                    pCollider->m_onBeginOverlap.Broadcast(pOtherCollider, pComponent->GetOwner());
                                    pOtherCollider->m_onBeginOverlap.Broadcast(pCollider, pColliderComponent->GetOwner());

                                    // If the components are still valid, then it is safe to add each of them to our list of
                                    // colliders to update overlaps on.
                                    if (pColliderComponent->m_collisionEnabled && pComponent->m_collisionEnabled)
                                    {
                                        // Add the overlapping colliders to eachother's overlapping colliders list.  
                                        pCollider->m_overlappingColliders.emplace(pOtherCollider);
                                        pOtherCollider->m_overlappingColliders.emplace(pCollider);

                                        // Add them to our list to update.
                                        m_overlappedCollidersToUpdate.emplace_back(pCollider);
                                        m_overlappedCollidersToUpdate.emplace_back(pOtherCollider);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Add a ColliderComponent to our array of actively collision checked colliders.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::AddActiveCollider(ColliderComponent* pColliderComponent)
    {
        // Make sure that our collider isn't already in our active collider array
        for(const auto* pActiveCollider : m_activeColliders)
        {
            if (pActiveCollider == pColliderComponent)
                return;
        }

        m_activeColliders.emplace_back(pColliderComponent);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Remove a ColliderComponent from our array of actively collision checked colliders.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::RemoveActiveCollider(ColliderComponent* pColliderComponent)
    {
        // If the collider was an active one, remove it from our active colliders.
        // O(n) but until it is an issue, I am not going to worry about it. But this is a pain point.
        for (size_t i = 0; i < m_activeColliders.size(); ++i)
        {
            if (m_activeColliders[i]->GetOwner() == pColliderComponent->GetOwner())
            {
                //MCP_LOG("Collision", "Removing Active Collider");
                std::swap(m_activeColliders[i], m_activeColliders.back());
                m_activeColliders.pop_back();
                break;
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Checks to see if every collider that was overlapping is still overlapping.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::UpdateOverlappingColliders()
    {
        // For each overlapping collider, we need to see if they are still overlapping.
        for (size_t i = 0; i < m_overlappedCollidersToUpdate.size();)
        {
            Collider*& pCollider = m_overlappedCollidersToUpdate[i];

            // If the ptr is invalid or the overlapping colliders is empty, then remove the
            // collider from our list and continue.
            if (!pCollider || pCollider->m_overlappingColliders.empty())
            {
                std::swap(m_overlappedCollidersToUpdate[i], m_overlappedCollidersToUpdate.back());
                m_overlappedCollidersToUpdate.pop_back();
                continue; // Continue without incrementing i, so that we don't skip the swapped element.
            }

            // For each of the overlapped components,
            for(auto iterator = pCollider->m_overlappingColliders.begin(); iterator != pCollider->m_overlappingColliders.end();)
            {
                // If the Collider is no longer valid,
                if ((*iterator) == nullptr)
                {
                    // Remove it and continue.
                    iterator = pCollider->m_overlappingColliders.erase(iterator);
                    continue;
                }

                // If we are no longer intersecting,
                if (!pCollider->GetEstimateRectWorld().Intersects((*iterator)->GetEstimateRectWorld()))
                {
                    // We need to send the OnOverlapExit events,
                    pCollider->m_onExitOverlap.Broadcast((*iterator), (*iterator)->m_pOwner->GetOwner());
                    (*iterator)->m_onExitOverlap.Broadcast(pCollider, pCollider->m_pOwner->GetOwner());

                    // Remove each other from their overlappingColliders
                    (*iterator)->m_overlappingColliders.erase(pCollider);
                    iterator = pCollider->m_overlappingColliders.erase(iterator);
                    continue;
                }

                // If we are still intersecting, send a OnOverlapUpdate event to both parties.
                pCollider->m_onOverlapUpdate.Broadcast((*iterator), (*iterator)->m_pOwner->GetOwner());
                (*iterator)->m_onOverlapUpdate.Broadcast(pCollider, pCollider->m_pOwner->GetOwner());

                ++iterator;
            }

            // If the overlapping colliders are now empty, then remove the collider from our list.
            if (pCollider->m_overlappingColliders.empty())
            {
                std::swap(m_overlappedCollidersToUpdate[i], m_overlappedCollidersToUpdate.back());
                m_overlappedCollidersToUpdate.pop_back();
                continue; 
            }

            // If the Collider still belongs in our array, then increment i and continue.
            ++i;
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      HACK.
    //		
    ///		@brief : Remove a collider from overlapping updates.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::RemoveOverlappingCollider(Collider* pCollider)
    {
        for (size_t i = 0; i < m_overlappedCollidersToUpdate.size(); ++i)
        {
            if (m_overlappedCollidersToUpdate[i] == pCollider)
            {
                //MCP_LOG("Collision", "Removing Overlapping Collider...");
                std::swap(m_overlappedCollidersToUpdate[i], m_overlappedCollidersToUpdate.back());
                m_overlappedCollidersToUpdate.pop_back();
                break;
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      TODO: I want to use this at some point. I would rather calculate the grid once up front. I started with this, but I moved away
    //      from it.
    //		
    ///		@brief : [NOT IMPLEMENTED YET] Calculate the Quadtree divisions for the world.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::CalculateGrid()
    {
        MCP_CHECK(m_pRoot);
        
        //SubDivide(m_pRoot,m_activeColliders, 0);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : See if the rect fits into the Cell, and if it is a Leaf, we will insert it into this Cell.
    ///             This will recurse for each child if this is not a leaf.
    ///		@param pCell : Cell we are trying to insert into.
    ///		@param rect : The rect that we are comparing. Should be pColliderComponent's 'EstimationRect'.
    ///		@param pColliderComponent : The Collider Component we are trying to add.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::TryInsert(QuadtreeCell* pCell, ColliderComponent* pColliderComponent, const RectF& rect)
    {
        // If we don't intersect, then return.
        if (!rect.Intersects(pCell->dimensions))
        {
            return;
        }

        // If inserting the Component into the leaf will cause it to be too large, then try to subdivide.
        if (IsLeaf(pCell) && pCell->m_colliderComponents.size() + 1 >= m_objectCountToTriggerDivide)
        {
            TrySubdivide(pCell);
        }

        // If we are still a leaf (we didn't subdivide), then insert the Collider here.
        if (IsLeaf(pCell))
        {
            pCell->m_colliderComponents.emplace_back(pColliderComponent);
            pColliderComponent->m_cells.emplace_back(pCell);
        }

        // If we aren't a leaf, then we need to try and insert into one of our children.
        else
        {
            // As we go further down the tree, if this is the smallest parent that contains us,
            // then we need to set it for the collider as a reference point.
            if (rect.IsInside(pCell->dimensions))
            {
                pColliderComponent->m_pReferenceCell = pCell;
            }

            for (size_t i = 0; i < 4; ++i)
            {
                TryInsert(pCell->children[i], pColliderComponent, rect);
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Attempt to subdivide a leaf cell; this only fails if we have already hit the max depth.
    ///             All colliders in pCell will be transferred into the appropriate children.
    ///		@param pCell : QuadtreeCell to divide.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::TrySubdivide(QuadtreeCell* pCell)
    {
        assert(IsLeaf(pCell));

        // If we have already dove too far, return.
        if (pCell->depth + 1 >= m_maxTreeDepth)
            return;

        static std::array<RectF, 4> childRects;

        // Set the dimensions of each child
        auto& parentRect = pCell->dimensions;
        const float childWidth = parentRect.width / 2.f;
        const float childHeight = parentRect.height / 2.f;

        // Top Left
        childRects[0] = { parentRect.x, parentRect.y, childWidth, childHeight};
        // Top Right
        childRects[1] = {parentRect.x + childWidth, parentRect.y, childWidth, childHeight};
        // Bottom Left
        childRects[2] = { parentRect.x, parentRect.y + childHeight, childWidth, childHeight};
        // Bottom Right
        childRects[3] = {parentRect.x + childWidth, parentRect.y + childHeight, childWidth, childHeight};

        // Create the Children.
        auto& children = pCell->children;
        for (size_t i = 0; i < 4; ++i)
        {
            children[i] = BLEACH_NEW(QuadtreeCell(pCell));
            children[i]->dimensions = childRects[i];
            children[i]->depth = pCell->depth + 1;

            // Move any colliders in the Cell that are completely inside the child Cell
            // into the new child.
            for(auto& m_colliderComponent : pCell->m_colliderComponents)
            {
                const RectF rect = m_colliderComponent->GetEstimationRect();
                TryInsert(children[i],  m_colliderComponent, rect);
            }
        }

        // Clear out the new parent cell's colliders. Only leaves have colliders.
        pCell->m_colliderComponents.clear();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Remove a Component from a cell.
    ///		@param pCell : Cell we are checking for the component.
    ///		@param pComponent : Component we are removing.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::RemoveFromCell(QuadtreeCell* pCell, const ColliderComponent* pComponent) const
    {
        if (pCell->m_colliderComponents.empty())
            return;

        //MCP_CHECK(IsLeaf(pCell));
        for (size_t i = 0; i < pCell->m_colliderComponents.size(); ++i)
        {
            if (pCell->m_colliderComponents[i]->GetOwner() == pComponent->GetOwner())
            {
                std::swap(pCell->m_colliderComponents[i], pCell->m_colliderComponents.back());
                pCell->m_colliderComponents.pop_back();
                return;
            }
        }

        MCP_WARN("Collision", "Failed to remove ColliderComponent from Cell!");
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      TODO: This is a problematic function because we store the collider components in an array, which we
    //      can't easily determine if we already have the collider in the node we are collapsing. If we want this to
    //      work nicely, we would need to store the colliders in a std::unordered_set or something.
    //		
    ///		@brief : Attempt to collapse child nodes into their parent if the number of colliders in the children is less
    ///             than the amount of allowable colliders.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::TryCleanup(QuadtreeCell* pCell) const
    {
        // If the cell is nullptr or we have a leaf, then return.
        if (!pCell || IsLeaf(pCell))
            return;

        // Check to see if the total number of Colliders in the child cells are less than the maximum
        // allowable colliders in a cell.
        size_t colliderCount = 0;
        for (size_t i = 0; i < 4; ++i)
        {
            colliderCount += pCell->children[i]->m_colliderComponents.size();

            // If we exceed the maximum size, then return, we can't cleanup the cell.
            if (colliderCount >= m_objectCountToTriggerDivide)
                return;
        }

        // If we have gotten here, then we have determined that the children have less that the maximum
        // allowable colliders in a cell, we can collapse the children into the parent.

        // For each child, copy their colliders into the parent, then delete the child.
        for (size_t i = 0; i < 4; ++i)
        {
            auto& colliderArray = pCell->children[i]->m_colliderComponents;

            for(auto& pColliderComponent : colliderArray)
            {
                pCell->m_colliderComponents.emplace_back(pColliderComponent);
            }

            // With the child emptied, we can delete the child.
            BLEACH_DELETE(pCell->children[i]);
            pCell->children[i] = nullptr;
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Clears and destroys each cell in the tree.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::ClearTree()
    {
        DeleteAllCells(m_pRoot);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Deletes 'pCell' and each QuadtreeCell attached to it.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::DeleteAllCells(QuadtreeCell* pCell)
    {
        if (pCell)
        {
            DeleteAllCells(pCell->children[0]);
            DeleteAllCells(pCell->children[1]);
            DeleteAllCells(pCell->children[2]);
            DeleteAllCells(pCell->children[3]);

            BLEACH_DELETE(pCell);
            pCell = nullptr;
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Removes the Component from any non intersecting cells, and adds the cells that it intersects with.
    ///		@param pColliderComponent : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::UpdateMembership(ColliderComponent* pColliderComponent)
    {
        const auto rect = pColliderComponent->GetEstimationRect();
        RemoveDanglingCells(pColliderComponent, rect);

        // If our collision is no longer enabled or we have been queued for deletion return.
        if (pColliderComponent->GetOwner()->IsQueuedForDeletion() || !pColliderComponent->CollisionEnabled())
            return;

        TryInsert(m_pRoot, pColliderComponent, rect);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      For now, I am going to do the easy thing and completely remove all cells that this collider was in.
    //      This could be changed to only remove the ones we are not intersecting with, but that will require changes to
    //      AddIntersecting Cells.
    //		
    ///		@brief : Remove cells that this component is no longer intersecting with.
    ///		@param pColliderComponent : Component we are updating.
    ///		@param colliderRect : EstimationRect for this colliderComponent.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::RemoveDanglingCells(ColliderComponent* pColliderComponent, [[maybe_unused]] const RectF& colliderRect) const
    {
        // Remove this collider from each cell that it intersects with.
        for (auto* pCell : pColliderComponent->m_cells)
        {
            RemoveFromCell(static_cast<QuadtreeCell*>(pCell), pColliderComponent);
        }

        // TODO LATER: For each cell that we have been removed from, attempt to clean up those cells'.

        pColliderComponent->m_cells.clear();
    }

    CollisionSystem::QuadtreeCell* CollisionSystem::FindLowestCellThatEncapsulatesRect(QuadtreeCell* pCell, const RectF& rect)
    {
        // If we were outside of the Tree, start by checking the root.
        if (!pCell)
            pCell = m_pRoot;

        QuadtreeCell* pParent = FindParentThatEncapsulatesRect(pCell, rect);

        // If there is no cell that encapsulates the Rect in our tree, return nullptr.
        if (!pParent)
            return nullptr;

        return FindChildThatEncapsulatesRect(pParent, rect);
    }

    CollisionSystem::QuadtreeCell* CollisionSystem::FindParentThatEncapsulatesRect(QuadtreeCell* pCell, const RectF& rect)
    {
        // If the rect is outside our root, return nullptr.
        if (!pCell)
            return nullptr;

        /*if (pCell == m_pRoot)
            return m_pRoot;*/

        // Check to see if the current cell encapsulates the Rect.
        if (rect.IsInside(pCell->dimensions))
        {
            return pCell;
        }

        // If we were not completely contained, then we need to move up the tree to find the parent
        // that does.
        return FindParentThatEncapsulatesRect(pCell->pParent, rect);
    }

    CollisionSystem::QuadtreeCell* CollisionSystem::FindChildThatEncapsulatesRect(QuadtreeCell* pCell, const RectF& rect)
    {
        MCP_CHECK(pCell);

        // If we are on a leaf, we can't go any further.
        if (IsLeaf(pCell))
            return pCell;

        // Otherwise, we need to check each child.
        for (size_t i = 0; i < 4; ++i)
        {
            QuadtreeCell* pChild = pCell->children[i];

            // If we have found a child that completely encapsulates the rect, 
            if (rect.IsInside(pChild->dimensions))
            {
                return FindChildThatEncapsulatesRect(pChild, rect);
            }
        }

        // If we have overlapped on multiple the children, then we return the current cell.
        return pCell;
    }


#if DEBUG_RENDER_COLLISION_TREE
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Renders the Partitions so that we can see how the world is divided up for collisions.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::Render() const
    {
        RenderEachCell(m_pRoot);
    }

    void CollisionSystem::RenderEachCell(const QuadtreeCell* pCell) const
    {
        static constexpr Color kBaseColor = Color{255, 0, 0,0};

        // If it isn't a leaf, then we want to continue going down.
        if (!IsLeaf(pCell))
        {
            RenderEachCell(pCell->children[0]);
            RenderEachCell(pCell->children[1]);
            RenderEachCell(pCell->children[2]);
            RenderEachCell(pCell->children[3]);
        }

        else
        {
            if (pCell->m_colliderComponents.empty())
            {
                DrawFillRect(pCell->dimensions, kBaseColor);
            }

            else
            {
                const auto alpha = std::clamp(static_cast<int>(pCell->m_colliderComponents.size()) * 10, 25, 180);
                Color color = kBaseColor;
                color.alpha = static_cast<uint8_t>(alpha);
                DrawFillRect(pCell->dimensions, color);
            }
        }
    }

#endif
}