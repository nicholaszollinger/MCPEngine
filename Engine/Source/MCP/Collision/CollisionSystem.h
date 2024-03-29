#pragma once
// CollisionSystem.h

#include <array>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Utility/Types/Rect.h"
#include "Utility/Types/Vector2.h"

#ifdef _DEBUG
    #define DEBUG_RENDER_COLLISION_TREE 0

#if DEBUG_RENDER_COLLISION_TREE
    #include "MCP/Scene/IRenderable.h"
    #include "Utility/Types/Color.h"
#endif
#else
#define DEBUG_RENDER_COLLISION_TREE 0
#endif

namespace mcp
{
    class Collider;
    class ColliderComponent;

    struct QuadtreeBehaviorData
    {
        unsigned int maxDepth          = 0;
        unsigned int maxObjectsInCell   = 0;
        float worldWidth                = 0.f;
        float worldHeight               = 0.f;
        float worldXPos                 = 0.f;
        float worldYPos                 = 0.f;
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : The Collision System is currently made to work as a uniform grid on a 2D surface.
    //-----------------------------------------------------------------------------------------------------------------------------
    class CollisionSystem
#if DEBUG_RENDER_COLLISION_TREE
        : public IRenderable
#endif
    {
        friend class WorldLayer;
        using ColliderId = Collider*;
        using Cell = std::unordered_set<size_t>;

    private:
        struct QuadtreeCell
        {
            QuadtreeCell(QuadtreeCell* pParent)
                : pParent(pParent)
            {
                //
            }

            std::array<QuadtreeCell*, 4> children {};
            std::vector<ColliderComponent*> m_colliderComponents {};
            RectF dimensions = {};
            QuadtreeCell* pParent = nullptr;
            unsigned depth = 0;
        };

#if DEBUG_RENDER_COLLISION_TREE
        static constexpr Color kTreeDebugColor = Color{255,255,0};
#endif
        
        std::vector<ColliderComponent*> m_activeColliders;
        std::vector<Collider*> m_overlappedCollidersToUpdate;
        QuadtreeCell* m_pRoot;
        size_t m_gridSize = 0;
        unsigned m_maxTreeDepth = 0;
        unsigned m_objectCountToTriggerDivide;
        unsigned m_treeDepth = 0;
        float m_worldWidth;
        float m_worldHeight;
        int m_cellWidth = 0;
        int m_cellHeight = 0;

    public:
        // Copying and Moving are NOT allowed.
        CollisionSystem(const CollisionSystem&) = delete;
        CollisionSystem(CollisionSystem&&) = delete;
        CollisionSystem& operator= (const CollisionSystem&) = delete;
        CollisionSystem& operator=(CollisionSystem&&) = delete;

        void SetQuadtreeBehaviorData(const QuadtreeBehaviorData& data);

        // Collider Registration.
        void AddCollideable(ColliderComponent* pColliderComponent);
        void RemoveCollideable(ColliderComponent* pColliderComponent);
        void SetCollideableStatic(ColliderComponent* pColliderComponent);
        void SetCollideableActive(ColliderComponent* pColliderComponent);
        void RemoveOverlappingCollider(Collider* pCollider);

        // Collision
        void RunCollisions();
        void CheckCollision(ColliderComponent* pColliderComponent);

    private:
        // Private Constructor. Only the Scene can create the collision System.
        CollisionSystem(const QuadtreeBehaviorData& data);

        // Destructor, virtual when debugging.
#if DEBUG_RENDER_COLLISION_TREE
        virtual ~CollisionSystem() override;
#else
        ~CollisionSystem();
#endif

    private:
        // Collision
        void RunCollisionForColliderComponent(ColliderComponent* pColliderComponent);
        void AddActiveCollider(ColliderComponent* pColliderComponent);
        void RemoveActiveCollider(ColliderComponent* pColliderComponent);
        void UpdateOverlappingColliders();

        // Quadtree
        static bool IsLeaf(const QuadtreeCell* cell) { return cell->children[0] == nullptr; }
        void UpdateMembership(ColliderComponent* pColliderComponent);
        void RemoveDanglingCells(ColliderComponent* pColliderComponent, const RectF& colliderRect) const;
        void TryInsert(QuadtreeCell* pCell, ColliderComponent* pColliderComponent, const RectF& rect);
        void RemoveFromCell(QuadtreeCell* pCell, const ColliderComponent* pComponent) const;
        void TrySubdivide(QuadtreeCell* pCell);
        void TryCleanup(QuadtreeCell* pCell) const;
        void ClearTree();
        void DeleteAllCells(QuadtreeCell* pCell);
        void CalculateGrid();

        // TO BE REMOVED...
        QuadtreeCell* FindLowestCellThatEncapsulatesRect(QuadtreeCell* pCell, const RectF& rect);
        QuadtreeCell* FindParentThatEncapsulatesRect(QuadtreeCell* pCell, const RectF& rect);
        QuadtreeCell* FindChildThatEncapsulatesRect(QuadtreeCell* pCell, const RectF& rect);

        // Debug Render functions.
#if DEBUG_RENDER_COLLISION_TREE
        virtual void Render() const final override;
        void RenderEachCell(const QuadtreeCell* pCell) const;
#endif
    };
}