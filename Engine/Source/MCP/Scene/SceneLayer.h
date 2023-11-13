#pragma once
// SceneLayer.h

#include "IRenderable.h"
#include "IUpdateable.h"
#include "SceneEntity.h"
#include "Utility/Types/Containers/UnorderedDenseArray.h"

namespace mcp
{
    class Scene;
    class SceneEntity;
    class ApplicationEvent;

#define MCP_DECLARE_LAYER_ENTITY_TYPE(EntityTypename)                                                       \
    friend class Scene;                                                                                     \
private:                                                                                                    \
    static constexpr mcp::LayerId kLayerId = EntityTypename::GetStaticLayerId();                            \
protected:                                                                                                  \
    static constexpr const char* kEntityElementTag = #EntityTypename;                                       \
    static constexpr uint32_t kEntityElementHash = HashString32(kEntityElementTag);                         \
    [[nodiscard]] virtual const char* GetEntityElementTag() const override { return kEntityElementTag; }    \
public:                                                                                                     \
    [[nodiscard]] static constexpr mcp::LayerId GetStaticLayerId() { return kLayerId; }                     \
    [[nodiscard]] virtual mcp::LayerId GetLayerId() const { return kLayerId; }                              \

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Base class for a Layer in the Scene like World, or UI. SceneLayers are Rendered and Updated from the bottom
    ///         layer to the top; they handle Events from the top down.
    //-----------------------------------------------------------------------------------------------------------------------------
    class SceneLayer
    {
        friend class Scene;
    protected:
        static constexpr uint32_t kSceneLayerAssetId = HashString32("SceneLayerAsset");

        std::unordered_map<EntityId, SceneEntity*> m_entities;                  // Container of all of the Entities in the scene.
        std::vector<EntityId> m_queuedEntitiesToDelete;                     // Entities that will be deleted at the end of the update.

        UnorderedDenseArray<UpdateableId, IUpdateable*> m_updateables;          // Anything that is updating on this layer.
        UnorderedDenseArray<UpdateableId, IUpdateable*> m_fixedUpdateables;     // Any physics based updateables that need to be updated in a fixed time.
        UnorderedDenseArray<RenderableId, IRenderable*> m_renderables;          // Anything that we need to render on this layer.
        Scene* m_pScene;                                                        // Reference to the Scene we are in.

    public:
        // Scene Layers cannot be copied or moved.
        SceneLayer(const SceneLayer&) = delete;
        SceneLayer& operator=(const SceneLayer&) = delete;
        SceneLayer(SceneLayer&&) = delete;
        SceneLayer& operator=(SceneLayer&&) = delete;

        // Entity Management
        virtual SceneEntity* CreateEntity() = 0;
        virtual SceneEntity* CreateEntityFromPrefab(const XMLElement root) = 0;
        void AddEntity(SceneEntity* pEntity);
        void DestroyEntity(const EntityId id);
        bool EntityExistsOnThisLayer(const EntityId id);

        // TODO: Time
        /*void Pause();
        void Resume();
        [[nodiscard]] bool IsPaused() const { return m_isPaused; }*/

        // Renderable Registration
        void AddRenderable(IRenderable* pRenderable);
        void RemoveRenderable(const IRenderable* pRenderable);

        // Updateable Registration
        void AddUpdateable(IUpdateable* pUpdateable);
        void RemoveUpdateable(const IUpdateable* pUpdateable);

        // Fixed Update Registration.
        void AddPhysicsUpdateable(IUpdateable* pUpdateable);
        void RemovePhysicsUpdateable(const IUpdateable* pUpdateable);

        [[nodiscard]] virtual LayerId GetLayerId() const = 0;
        [[nodiscard]] Scene* GetScene() const { return m_pScene; }

        template<typename LayerType>
        static LayerType* SafeCastLayer(SceneLayer* pLayer);

        template<typename LayerType>
        static const LayerType* SafeCastLayer(const SceneLayer* pLayer);

    protected:
        // Private Constructor and Destructor
        SceneLayer(Scene* pScene);
        virtual ~SceneLayer() = default;

        // Layer Loading and Unloading
        virtual bool LoadLayer(const XMLElement layer);
        virtual bool OnSceneLoad() { return true; }
        virtual void LoadEntity(XMLElement element) = 0;
        void LoadLayerAssetsAndEntities(const XMLElement element);
        void LoadSceneLayerAsset(const XMLElement sceneLayerAssetElement);
        void DestroyLayer();

        // Scene Layer Loop
        virtual void Update(const float deltaTimeMs) = 0;
        virtual void FixedUpdate(const float fixedUpdateTimeMs) = 0;
        virtual void Render() = 0;
        virtual void OnEvent(ApplicationEvent& pEvent) = 0;
        void DeleteQueuedEntities();

        [[nodiscard]] virtual const char* GetEntityElementTag() const = 0;
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the SceneLayer cast to the child SceneLayer type. In debug, this will assert that the cast is valid.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename LayerType>
    LayerType* SceneLayer::SafeCastLayer(SceneLayer* pLayer)
    {
        MCP_CHECK(pLayer->GetLayerId() == LayerType::GetStaticLayerId());
        return static_cast<LayerType*>(pLayer);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the SceneLayer cast to the child SceneLayer type. In debug, this will assert that the cast is valid.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename LayerType>
    const LayerType* SceneLayer::SafeCastLayer(const SceneLayer* pLayer)
    {
        MCP_CHECK(pLayer->GetLayerId() == LayerType::GetStaticLayerId());
        return static_cast<const LayerType*>(pLayer);
    }
}

