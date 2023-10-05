#pragma once
// SceneLayer.h

#include "IRenderable.h"
#include "IUpdateable.h"
#include "Object.h"
#include "Utility/Types/Containers/UnorderedDenseArray.h"

namespace mcp
{
    class Scene;
    class ApplicationEvent;

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Base class for a Layer in the Scene like World, or UI. SceneLayers are Rendered and Updated from the bottom
    ///         layer to the top; they handle Events from the top down.
    //-----------------------------------------------------------------------------------------------------------------------------
    class SceneLayer
    {
    protected:
        static constexpr uint32_t kSceneLayerAssetId = HashString32("SceneLayerAsset");

        UnorderedDenseArray<UpdateableId, IUpdateable*> m_updateables;          // Anything that is updating on this layer.
        UnorderedDenseArray<UpdateableId, IUpdateable*> m_fixedUpdateables;     // Any physics based updateables that need to be updated in a fixed time.
        UnorderedDenseArray<RenderableId, IRenderable*> m_renderables;          // Anything that we need to render on this layer.
        Scene* m_pScene;                                                        // Reference to the Scene we are in.

    public:
        SceneLayer(Scene* pScene);
        virtual ~SceneLayer() = default;

        SceneLayer(const SceneLayer&) = delete;
        SceneLayer& operator=(const SceneLayer&) = delete;
        SceneLayer(SceneLayer&&) = delete;
        SceneLayer& operator=(SceneLayer&&) = delete;

        virtual bool LoadLayer(const XMLElement layer) = 0;
        virtual void Update(const float deltaTimeMs) = 0;
        virtual void FixedUpdate(const float fixedUpdateTimeMs) = 0;
        virtual void Render() = 0;
        virtual bool OnSceneLoad() { return true; }
        virtual void OnEvent(ApplicationEvent& pEvent) = 0;

        // Renderable Registration
        void AddRenderable(IRenderable* pRenderable);
        void RemoveRenderable(const IRenderable* pRenderable);

        // Updateable Registration
        void AddUpdateable(IUpdateable* pUpdateable);
        void RemoveUpdateable(const IUpdateable* pUpdateable);

        // Fixed Update Registration.
        void AddPhysicsUpdateable(IUpdateable* pUpdateable);
        void RemovePhysicsUpdateable(const IUpdateable* pUpdateable);

    protected:
        virtual void LoadSceneDataAsset(const XMLElement sceneDataAsset) = 0;
        virtual void DestroyLayer() = 0;
    };
}

