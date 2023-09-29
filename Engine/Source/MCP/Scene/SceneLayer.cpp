// SceneLayer.cpp

#include "SceneLayer.h"

namespace mcp
{
    // TODO: These are magic numbers! These should be set in Data! (Also maybe ditch the data structure...)
    SceneLayer::SceneLayer(Scene* pScene)
        : m_updateables(64)
        , m_fixedUpdateables(64)
        , m_renderables(64)
        , m_pScene(pScene)
    {
        //
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Add a renderable to this layer.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneLayer::AddRenderable(IRenderable* pRenderable)
    {
        m_renderables.Add(pRenderable->GetRenderId(), pRenderable);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Remove a renderable on this layer.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneLayer::RemoveRenderable(const IRenderable* pRenderable)
    {
        m_renderables.Remove(pRenderable->GetRenderId());
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Add an updateable that will be ticked every frame.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneLayer::AddUpdateable(IUpdateable* pUpdateable)
    {
        m_updateables.Add(pUpdateable->GetUpdateId(), pUpdateable);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Remove an updateable that was ticking every frame.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneLayer::RemoveUpdateable(const IUpdateable* pUpdateable)
    {
        m_updateables.Remove(pUpdateable->GetUpdateId());
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Add an updateable that will only tick on a fixed interval.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneLayer::AddPhysicsUpdateable(IUpdateable* pUpdateable)
    {
        m_fixedUpdateables.Add(pUpdateable->GetUpdateId(), pUpdateable);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Remove an updateable that was only ticking on a fixed interval. You would to have needed to call 'AddPhysicsUpdateable'
    ///         for this function to succeed.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneLayer::RemovePhysicsUpdateable(const IUpdateable* pUpdateable)
    {
        m_fixedUpdateables.Remove(pUpdateable->GetUpdateId());
    }
}