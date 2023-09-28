#pragma once
// Scene.h

#include "IRenderable.h"
#include "IUpdateable.h"
#include "MCP/Collision/CollisionSystem.h"
#include "MCP/Core/Event/MessageManager.h"
#include "MCP/Scene/Object.h"
#include "Utility/Types/Containers/UnorderedDenseArray.h"

namespace mcp
{
    class Scene
    {

    private:
        using RenderableContainer = UnorderedDenseArray<RenderableId, IRenderable*>;

        static constexpr const char* kPackageElementName = "Package";
        static constexpr const char* kSceneElementName = "Scene";
        static constexpr const char* kObjectElementName = "Object";
        static constexpr float kFixedUpdateTimeSeconds = 1.f / 60.f;
        //static constexpr size_t kRenderLayerSize = static_cast<size_t>(RenderLayer::kRenderCount);

        // TODO: This isn't going to work for renderables. I need to be able to sort them.
        UnorderedDenseArray<IUpdateableId, IUpdateable*> m_updateables; // Anything that is updating in our scene.
        UnorderedDenseArray<IUpdateableId, IUpdateable*> m_fixedUpdateables;    // Any physics based updateables that need to be updated in a fixed time.

        // Hard coded Render layers???? Doesn't feel great.
        RenderableContainer m_worldRenderables;
        RenderableContainer m_objectRenderables;
        RenderableContainer m_debugOverlayRenderables;

        std::unordered_map<ObjectId, Object*> m_objects;            // Container of all of the Object's in the scene.
        std::vector<ObjectId> m_queuedObjectsToDelete;               // Objects that will be deleted at the end of the update.
        CollisionSystem m_collisionSystem;
        MessageManager m_messageManager;
        float m_accumulatedTime;                                    // Amount of time before we perform a fixed update.
        bool m_transitionQueued;                                    // Whether a scene transition has been queued or not.

    public:
        Scene();
        ~Scene();

        bool Init();
        bool Load(const char* pFilePath);
        void Unload() {}
        bool OnSceneLoad();
        void Update(const float deltaTime);
        void Render() const;

        Object* CreateObject();
        void DestroyObject(const ObjectId id);
        [[nodiscard]] bool IsValidId(const ObjectId id);

        void AddRenderable(IRenderable* pRenderable);
        void RemoveRenderable(const IRenderable* pRenderable);

        void AddUpdateable(const IUpdateableId id, IUpdateable* pUpdateable);
        void RemoveUpdateable(const IUpdateableId id);

        void AddPhysicsUpdateable(IUpdateable* pUpdateable);
        void RemovePhysicsUpdateable(const IUpdateable* pUpdateable);

        [[nodiscard]] MessageManager* GetMessageManager() { return &m_messageManager;}
        [[nodiscard]] CollisionSystem* GetCollisionSystem() { return &m_collisionSystem;}

    private:
        void RenderLayer(const RenderableContainer& renderables) const;
        void DeleteQueuedObjects();
        void ClearScene();
        void SetCollisionSettings(const QuadtreeBehaviorData& data);
    };
}