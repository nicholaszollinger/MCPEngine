// CollisionSystem.cpp

#include "CollisionSystem.h"
#include "Box2DCollider.h"
#include "Collider.h"
#include "MCP/Components/ColliderComponent.h"

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
        // Get the last cell that completely encompassed this pColliderComponent was in.
        QuadtreeCell* pCell = static_cast<QuadtreeCell*>(pColliderComponent->m_pCell);

        // If did not have a cell, then our starting point will be the root.
        if (!pCell)
        {
            pCell = m_pRoot;
        }

        const RectF estimationRect = pColliderComponent->GetEstimationRect();
        TryInsert(pCell, estimationRect, pColliderComponent);

        if (!pColliderComponent->m_pCell)
            pColliderComponent->m_pCell = m_pRoot;

        // TODO: Insert this into a list of colliders if this collider component was active.
        if (!pColliderComponent->m_isStatic)
            m_activeColliders.emplace_back(pColliderComponent);

        // This is to ensure that I have completed the 'registration'.
        assert(pColliderComponent->m_pCell);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Remove a ColliderComponent from the tree.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::RemoveCollideable(const ColliderComponent* pColliderComponent)
    {
        auto* pCell = static_cast<QuadtreeCell*> (pColliderComponent->m_pCell);
        assert(pCell);

        const RectF rect = pColliderComponent->GetEstimationRect();
        std::vector<QuadtreeCell*> cells;

        if (!rect.IsInside(pCell->dimensions))
        {
            pCell = FindParentThatEncapsulatesRect(pCell, rect);
        }

        FindCellsForRect(pCell, rect, cells);
        for(auto* pCellToRemoveFrom : cells)
        {
            RemoveFromCell(pCellToRemoveFrom, pColliderComponent);
        }

        // If the collider was an active one, remove it from our active colliders.
        // O(n) but until it is an issue, I am not going to worry about it. But this is a pain point.
        for (size_t i = 0; i < m_activeColliders.size(); ++i)
        {
            if (m_activeColliders[i]->m_pOwner == pColliderComponent->m_pOwner)
            {
                std::swap(m_activeColliders[i], m_activeColliders.back());
                m_activeColliders.pop_back();
                break;
            }
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
    ///		@brief : If the collider component is no longer active, we will remove it from the 'active' colliders that are
    ///         updated each frame. Else, we will add it to the list.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::SetCollideableStatic(ColliderComponent* pColliderComponent, const bool bIsStatic)
    {
        size_t activeColliderIndex = 0;
        for (; activeColliderIndex != m_activeColliders.size(); ++activeColliderIndex)
        {
            if (m_activeColliders[activeColliderIndex]->m_pOwner == pColliderComponent->m_pOwner)
            {
                break;
            }
        }

        // If the collider component is now static, we need to remove it from our active colliders.
        if (bIsStatic)
        {
            if (activeColliderIndex < m_activeColliders.size())
            {
                std::swap(m_activeColliders[activeColliderIndex], m_activeColliders.back());
                m_activeColliders.pop_back();
            }
        }

        else
        {
            if (activeColliderIndex == m_activeColliders.size())
            {
                m_activeColliders.emplace_back(pColliderComponent);
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Test the Collision of a ColliderComponent after a move.
    ///		@param  pColliderComponent : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::CheckCollision(ColliderComponent* pColliderComponent)
    {
        auto* pCell = static_cast<QuadtreeCell*> (pColliderComponent->m_pCell);

        assert(pCell);
        assert(pColliderComponent->m_collisionEnabled); // We shouldn't be in the tree if our collision is disabled.

        // See if we are still in the same cell.
        const RectF rect = pColliderComponent->GetEstimationRect();

        // If we aren't, then find the cell that encapsulates the cell.
        if (!rect.IsInside(pCell->dimensions))
        {
            pCell = FindLowestCellThatEncapsulatesRect(pCell, rect);
        }

        // Find all of the cells that this collider component is in, using our reference point.
        std::vector<QuadtreeCell*> cells;
        FindCellsForRect(pCell, rect, cells);

        // Run the collision.
        RunCollisionForColliderComponent(cells, pColliderComponent);

        // I am going to do what is easy and remove the collider from each cell it was in and reinsert
        // to update the membership.
        for (auto* pOldCell : cells)
        {
            RemoveFromCell(pOldCell, pColliderComponent);
        }

        // Try inserting again, from our reference point.
        TryInsert(pCell, rect, pColliderComponent);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Takes in the cells that the Collider Component is in and Checks for collisions in each cell.
    ///		@param cells : Cells that the Collider Component is in.
    ///		@param pColliderComponent : Component we are testing.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::RunCollisionForColliderComponent(const std::vector<QuadtreeCell*>& cells, ColliderComponent* pColliderComponent)
    {
        const RectF& estimationRect = pColliderComponent->GetEstimationRect();

        for (auto* pCell : cells)
        {
            // If we have 1 or less components in this cell, then continue.
            if (pCell->m_colliderComponents.size() <= 1)
                continue;

            // For each other collider component, we need to see if we collide.
            for (auto& pComponent : pCell->m_colliderComponents)
            {
                // Don't check against itself.
                if (pComponent->m_pOwner == pColliderComponent->m_pOwner)
                    continue;

                // Make sure this component is still valid for collision. Collisions may result in objects being destroyed,
                // colliders may be turned off, etc.
                if (!pComponent->m_collisionEnabled)
                    continue;

                // If the bounding boxes of the ColliderComponents don't intersect, then we can leave.
                if (!pComponent->GetEstimationRect().Intersects(estimationRect))
                    continue;

                // If they do intersect, then we need to check our individual colliders.
                //      NOTE: I am assuming *only* Box2DColliders which are *axis aligned*.
                for (auto& [name, pCollider] : pColliderComponent->m_colliders)
                {
                    const RectF myEstimateRect = pCollider->GetEstimateRectWorld();

                    // For each active collider in pComponent
                    for (auto& [otherName, pOtherCollider] : pComponent->m_colliders)
                    {
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

                                pColliderComponent->GetTransformComponent()->AddToLocationNoUpdate(deltaPos);
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
    //      TODO: I want to use this at some point. I would rather calculate the grid once up front.
    //		
    ///		@brief : [NOT IMPLEMENTED YET] Calculate the Quadtree divisions for the world.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::CalculateGrid()
    {
        assert(m_pRoot);
        
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
    void CollisionSystem::TryInsert(QuadtreeCell* pCell, const RectF& rect, ColliderComponent* pColliderComponent)
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
        }

        // If we aren't a leaf, then we need to try and insert into one of our children.
        else
        {
            // As we go further down the tree, if this is the smallest parent that contains us,
            // then we need to set it for the collider as a reference point.
            if (rect.IsInside(pCell->dimensions))
            {
                pColliderComponent->m_pCell = pCell;
            }

            for (size_t i = 0; i < 4; ++i)
            {
                TryInsert(pCell->children[i], rect, pColliderComponent);
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
                TryInsert(children[i], rect, m_colliderComponent);
            }
        }

        // Clear out the new parent cell's colliders. Only leaves have colliders.
        pCell->m_colliderComponents.clear();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Remove a Component from a cell. This is called in conjunction with 'FindCellsForRect'
    ///		@param pCell : Cell we are checking for the component.
    ///		@param pComponent : Component we are removing.
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::RemoveFromCell(QuadtreeCell* pCell, const ColliderComponent* pComponent) const
    {
        //assert(IsLeaf(pCell));

        for (size_t i = 0; i < pCell->m_colliderComponents.size(); ++i)
        {
            if (pCell->m_colliderComponents[i]->m_pOwner == pComponent->m_pOwner)
            {
                std::swap(pCell->m_colliderComponents[i], pCell->m_colliderComponents.back());
                pCell->m_colliderComponents.pop_back();
                break;
            }
        }
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
    ///		@brief : Get each leaf cell that the rect belongs to in the tree.
    ///		@param pCell : 
    ///		@param rect : 
    ///		@param outCells : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void CollisionSystem::FindCellsForRect(QuadtreeCell* pCell, const RectF& rect, std::vector<QuadtreeCell*>& outCells)
    {
        // If we don't intersect with this cell, then return.
        if (!rect.Intersects(pCell->dimensions))
        {
            return;
        }

        // If we intersect and this leaf
        if (IsLeaf(pCell))
        {
            outCells.push_back(pCell);
        }

        // If we intersect but this isn't a leaf, then we need to find the leaf that
        // we intersect with.
        else
        {
            for (size_t i = 0; i < 4; ++i)
            {
                FindCellsForRect(pCell->children[i], rect, outCells);
            }
        }
    }

    CollisionSystem::QuadtreeCell* CollisionSystem::FindLowestCellThatEncapsulatesRect(QuadtreeCell* pCell, const RectF& rect)
    {
        QuadtreeCell* pParent = FindParentThatEncapsulatesRect(pCell, rect);
        return FindChildThatEncapsulatesRect(pParent, rect);
    }

    CollisionSystem::QuadtreeCell* CollisionSystem::FindParentThatEncapsulatesRect(QuadtreeCell* pCell, const RectF& rect)
    {
        if (pCell == m_pRoot)
            return m_pRoot;

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
        assert(pCell);

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
        // If it isn't a leaf, then we want to continue going down.
        if (!IsLeaf(pCell))
        {
            RenderEachCell(pCell->children[0]);
            RenderEachCell(pCell->children[1]);
            RenderEachCell(pCell->children[2]);
            RenderEachCell(pCell->children[3]);

            for (const auto* pChild : pCell->children)
            {
                DrawRect(pChild->dimensions, kTreeDebugColor);
            }
        }
    }

#endif
}
