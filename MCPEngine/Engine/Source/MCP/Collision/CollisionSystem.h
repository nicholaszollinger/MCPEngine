#pragma once
// CollisionSystem.h

#include <array>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "utility/Rect.h"
#include "utility/Vector2.h"

#ifdef _DEBUG
    #define DEBUG_RENDER_COLLISION_TREE 1

#if DEBUG_RENDER_COLLISION_TREE
    #include "MCP/Scene/IRenderable.h"
    #include "utility/Color.h"
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
        unsigned int maxHeight          = 0;
        unsigned int maxObjectsInCell   = 0;
        float worldWidth                = 0.f;
        float worldHeight               = 0.f;
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
        friend class Scene;
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
            RectF dimensions;
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

        // Collider Registration.
        void AddCollideable(ColliderComponent* pColliderComponent);
        void RemoveCollideable(const ColliderComponent* pColliderComponent);
        void SetCollideableStatic(ColliderComponent* pColliderComponent, const bool bIsStatic);

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
        void RunCollisionForColliderComponent(const std::vector<QuadtreeCell*>& cells, ColliderComponent* pColliderComponent);
        void UpdateOverlappingColliders();

        // Quadtree
        static bool IsLeaf(const QuadtreeCell* cell) { return cell->children[0] == nullptr; }
        static void FindCellsForRect(QuadtreeCell* pCell, const RectF& rect, std::vector<QuadtreeCell*>& outCells);
        void TryInsert(QuadtreeCell* pCell, const RectF& rect, ColliderComponent* pColliderComponent);
        void TrySubdivide(QuadtreeCell* pCell);
        void RemoveFromCell(QuadtreeCell* pCell, const ColliderComponent* pComponent) const;
        void TryCleanup(QuadtreeCell* pCell) const;
        void DeleteAllCells(QuadtreeCell* pCell);
        void ClearTree();
        void CalculateGrid();
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
