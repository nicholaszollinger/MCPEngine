#pragma once
// Scene.h

#include "IRenderable.h"
#include "IUpdateable.h"
#include "MCP/Collision/CollisionSystem.h"
#include "MCP/Core/Event/MessageManager.h"
#include "MCP/Scene/Object.h"
#include "MCP/Scene/WorldLayer.h"
//#include "Utility/Types/Containers/UnorderedDenseArray.h"

namespace mcp
{
    class UILayer final : public SceneLayer
    {
        // Widget* pRoot;

    public:
        UILayer(Scene* pScene) : SceneLayer(pScene) {}

        virtual void Update([[maybe_unused]] const float deltaTimeMs) override {}
        virtual void FixedUpdate([[maybe_unused]] const float fixedUpdateTimeMs) override {}
        virtual void Render() override {}
        virtual void OnEvent([[maybe_unused]] const ApplicationEvent& event) override {}
    };

    class Scene
    {
    private:
        WorldLayer* m_pWorldLayer;
        UILayer* m_pUILayer;

        using RenderableContainer = UnorderedDenseArray<RenderableId, IRenderable*>;

        static constexpr const char* kPackageElementName = "Package";
        static constexpr const char* kSceneElementName = "Scene";
        static constexpr const char* kObjectElementName = "Object";
        static constexpr float kFixedUpdateTimeSeconds = 1.f / 60.f;
        //static constexpr size_t kRenderLayerSize = static_cast<size_t>(RenderLayer::kRenderCount);

        // TODO: This isn't going to work for renderables. I need to be able to sort them.
        //UnorderedDenseArray<IUpdateableId, IUpdateable*> m_updateables; // Anything that is updating in our scene.
        //UnorderedDenseArray<IUpdateableId, IUpdateable*> m_fixedUpdateables;    // Any physics based updateables that need to be updated in a fixed time.

        // Hard coded Render layers???? Doesn't feel great.
        //RenderableContainer m_worldRenderables;
        //RenderableContainer m_objectRenderables;
        //RenderableContainer m_debugOverlayRenderables;

        //std::unordered_map<UpdateableId, Object*> m_objects;            // Container of all of the Object's in the scene.
        //std::vector<UpdateableId> m_queuedObjectsToDelete;               // Objects that will be deleted at the end of the update.
        //CollisionSystem m_collisionSystem; // - World
        //MessageManager m_messageManager;   // - World
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

        void AddUpdateable(const UpdateableId id, IUpdateable* pUpdateable);
        void RemoveUpdateable(const UpdateableId id);

        void AddPhysicsUpdateable(IUpdateable* pUpdateable);
        void RemovePhysicsUpdateable(const IUpdateable* pUpdateable);

        [[nodiscard]] MessageManager* GetMessageManager();
        [[nodiscard]] CollisionSystem* GetCollisionSystem();

        [[nodiscard]] WorldLayer* GetWorldLayer() const { return m_pWorldLayer; }
        [[nodiscard]] UILayer* GetUILayer() const { return m_pUILayer; }

        [[nodiscard]] bool TransitionQueued() const { return m_transitionQueued; }

    private:
        void RenderLayer(const RenderableContainer& renderables) const;
        void DeleteQueuedObjects();
        void ClearScene();
        void SetCollisionSettings(const QuadtreeBehaviorData& data);

        void OnEvent(const ApplicationEvent& event) const;
    };
}