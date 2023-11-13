// SceneLayer.cpp

#include "SceneLayer.h"

#include "SceneAsset.h"

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
    ///		@brief : Add a previously created entity to this Layer.
    ///		@param pEntity : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneLayer::AddEntity(SceneEntity* pEntity)
    {
        if (!pEntity)
            return;

        // If the Entity does not belong on this layer, then return
        if (pEntity->GetLayerId() != GetLayerId())
        {
            MCP_WARN("SceneLayer", "Attempted to add a SceneEntity that doesn't belong to this SceneLayer!");
            return;
        }

        // If we already have this entity, return.
        if (EntityExistsOnThisLayer(pEntity->GetId()))
        {
            MCP_WARN("SceneLayer", "Attempted to add a SceneEntity is already on this SceneLayer!");
            return;
        }

        // Add the Entity to this layer.
        m_entities.emplace(pEntity->GetId(), pEntity);

        // Set their layer.
        pEntity->SetLayer(this);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Queue the deletion of an Entity. Entities are deleted at the end of the frame.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneLayer::DestroyEntity(const EntityId id)
    {
        const auto result = m_entities.find(id);
        if (result == m_entities.end())
        {
            MCP_WARN("SceneLayer", "Failed to DestroyEntity! EntityId was not found in layer!");
            return;
        }

        // Ensure that they are ready to be destroyed.
        result->second->Destroy();

        // Add the Entity to our deletion queue to be deleted on the next frame update:
        m_queuedEntitiesToDelete.emplace_back(id);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns whether the EntityId has an owner in this layer. If no SceneEntity exists, this returns false.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool SceneLayer::EntityExistsOnThisLayer(const EntityId id)
    {
        if (const auto result = m_entities.find(id); result == m_entities.end())
        {
            return false;
        }

        return true;
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

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Load the Layer from an XML file. The default behavior is to load all Layer assets and entities from the first
    ///         child element of the 'layer' element.
    ///		@param layer : The root element for this layer.
    ///		@returns : False if unsuccessful.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool SceneLayer::LoadLayer(const XMLElement layer)
    {
        const XMLElement element = layer.GetChildElement();
        LoadLayerAssetsAndEntities(element);

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Destroys all Entities in the SceneLayer.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneLayer::DestroyLayer()
    {
        // Delete any entities that were scheduled to be cleaned up.
        DeleteQueuedEntities();

        // Delete all of our entities.
        for(auto& [id, pEntity] : m_entities)
        {
            if (!pEntity->IsQueuedForDeletion())
                pEntity->Destroy();
        }

        DeleteQueuedEntities();

        m_entities.clear();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : From the Element, this will load any Scene
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneLayer::LoadLayerAssetsAndEntities(XMLElement element)
    {
        while(element.IsValid())
        {
            // If we hit another Scene Data Asset, load that asset
            if (HashString32(element.GetName()) == kSceneLayerAssetId)
            {
                LoadSceneLayerAsset(element);
            }

            // Otherwise, load that sceneEntity for this Layer
            else
            {
                //MCP_CHECK(HashString32(element.GetName()) == HashString32(GetEntityElementTag()));
                LoadEntity(element);   
            }

            element = element.GetSiblingElement();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Loads a SceneLayerAsset from an XMLFile. This will load all SceneEntities and other SceneLayerAssets in this asset.
    ///		@param sceneLayerAssetElement : The SceneLayerAsset element in a XML file.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneLayer::LoadSceneLayerAsset(const XMLElement sceneLayerAssetElement)
    {
        // In release, we check to see if the Asset is only for debug.
#ifndef _DEBUG
        if (AssetIsDebugOnly(sceneLayerAssetElement))
        {
            return;
        }
#endif

        // Get the Path
        const char* pPath = sceneLayerAssetElement.GetAttributeValue<const char*>("path");
        MCP_CHECK_MSG(pPath, "Failed to load SceneDataAsset! No path was found!");

        // Load the file
        XMLParser parser;
        if (!parser.LoadFile(pPath))
        {
            MCP_ERROR("SceneLayer", "Failed to load SceneDataAsset at path: ", pPath);
            return;
        }

        // Load the Scene Data asset.
        const XMLElement firstElementInAsset = parser.GetElement();
        LoadLayerAssetsAndEntities(firstElementInAsset);
    }

    void SceneLayer::DeleteQueuedEntities()
    {
        for (auto id : m_queuedEntitiesToDelete)
        {
            const auto result = m_entities.find(id);
            if (result == m_entities.end())
                continue;

            // Delete the entity
            BLEACH_DELETE(result->second);

            // Erase them from our container.
            m_entities.erase(result);
        }

        m_queuedEntitiesToDelete.clear();
    }

}
