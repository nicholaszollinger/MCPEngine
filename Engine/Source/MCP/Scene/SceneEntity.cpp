// SceneEntity.cpp

#include "SceneEntity.h"

#include "MCP/Debug/Log.h"
#include "MCP/Lua/Lua.h"
#include "MCP/Scene/Scene.h"
#include "MCP/Scene/SceneLayer.h"

namespace mcp
{
    SceneEntity::SceneEntity()
        : m_pParent(nullptr)
        , m_id(s_idCounter++)
        , m_isActive(true)
    {
        //
    }

    SceneEntity::SceneEntity(const SceneEntityConstructionData& data)
        : m_pParent(nullptr)
        , m_id(s_idCounter++)
        , m_tag(data.tag)
        , m_isActive(data.startActive)
    {
        //
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the Parent of this SceneEntity. 
    ///		@param pEntity : The new parent. This can be nullptr.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneEntity::SetParent(SceneEntity* pEntity)
    {
        if (pEntity == m_pParent)
            return;

        // If we already have a parent, we need to remove ourselves as a child.
        if (m_pParent)
        {
            m_pParent->RemoveChild(this);
        }

        m_pParent = pEntity;

        // If pEntity isn't nullptr, we need to add ourselves as a child
        if (m_pParent)
        {
            m_pParent->m_children.emplace_back(this);

            // Have the parent respond to the change
            m_pParent->OnChildAdded(this);

            // Have the entity respond to the change.
            OnParentActiveChanged(m_pParent->IsActive());
            OnParentSet();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Add a child to this SceneEntity
    ///		@param pEntity : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneEntity::AddChild(SceneEntity* pEntity)
    {
        // Make sure that we don't already have the child trying to be added.
        for (const auto* pChild : m_children)
        {
            if (pChild == pEntity)
                return;
        }

        // If our our new child had a parent, remove it from them.
        if (pEntity->m_pParent)
            pEntity->m_pParent->RemoveChild(pEntity);

        auto* pNewChild = m_children.emplace_back(pEntity);

        pNewChild->m_pParent = this;
        pNewChild->OnParentSet();
        pNewChild->OnParentActiveChanged(IsActive());

        // Respond to the addition.
        OnChildAdded(pNewChild);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Remove a Child from this SceneEntity.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneEntity::RemoveChild(SceneEntity* pEntity)
    {
        for (size_t i = 0; i < m_children.size(); ++i)
        {
            if (pEntity == m_children[i])
            {
                std::swap(m_children[i], m_children.back());
                m_children.pop_back();
                OnChildRemoved(pEntity);
                return;
            }
        }

        MCP_WARN("SceneEntity", "Tried to remove a child that doesn't exist on the parent!");
    }

    SceneEntity* SceneEntity::GetChildByTag(const StringId tag)
    {
        for (auto* pChild : m_children)
        {
            if (pChild->GetTag() == tag)
            {
                return pChild;
            }

            // Recursively check their children:
            if (auto* pFound = pChild->GetChildByTag(tag))
                return pFound;
        }

        return nullptr;
    }

    SceneEntity* SceneEntity::GetChildByTag(const StringId tag) const
    {
        for (auto* pChild : m_children)
        {
            if (pChild->GetTag() == tag)
            {
                return pChild;
            }

            // Recursively check their children:
            if (auto* pFound = pChild->GetChildByTag(tag))
                return pFound;
        }

        return nullptr;
    }


    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Queue the destruction of this SceneEntity. This will remove itself from the parent-child tree; All children of
    ///         this widget will have their Parents set to our parent.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneEntity::Destroy()
    {
        // If we are already queued for deletion, return.
        if (m_isQueuedForDeletion)
            return;

        // Queue ourselves.
        m_isQueuedForDeletion = true;

        // Queue the Destruction of each child.
        for (auto* pChild : m_children)
        {
            pChild->Destroy();
        }

        // Set the Entity to inactive.
        SetActive(false);

        // Let the Entity respond to the deletion.
        OnDestroy();

        // Queue the Entity for deletion.
        GetLayer()->DestroyEntity(m_id);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the SceneEntity's active state. If this has children, it will notify them of the change recursively.
    ///		@param isActive : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneEntity::SetActive(const bool isActive)
    {
        // If our status isn't changing, return.
        if (isActive == m_isActive)
            return;

        m_isActive = isActive;

        // If we have a parent, we need to check with their active state before updating our active state.
        if (m_pParent)
        {
            if (m_pParent->IsActive())
            {
                // If our parent was active and our state has changed, then we need to update.
                if (m_isActive)
                    OnActive();

                else
                    OnInactive();
            }
        }

        // Otherwise we update according to our active state.
        else
        {
            // If we have been set active.
            if (m_isActive)
                OnActive();

            // If we have been turned off:
            else 
                OnInactive();
            
        }

        // Let our children know of the change:
        for (auto* pChild : m_children)
        {
            pChild->OnParentActiveChanged(m_isActive);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Toggles the active state of this SceneEntity.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneEntity::ToggleActive()
    {
        SetActive(!m_isActive);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns if the SceneEntity is active or not. If a parent SceneEntity is inactive, then this will return false.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool SceneEntity::IsActive() const
    {
        // If I have a parent and the the parent is inactive, return false.
        if (m_pParent && !m_pParent->IsActive())
            return false;

        return m_isActive;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Respond to our parent's active state changing, and let all of our children know recursively.
    ///		@param parentActiveState : The new state of the parent.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneEntity::OnParentActiveChanged(const bool parentActiveState)
    {
        // If were active and our parent has changed, we need to respond.
        if (m_isActive)
        {
            if (parentActiveState)
                OnActive();

            else
                OnInactive();
        }

        for (auto* pChild : m_children)
        {
            pChild->OnParentActiveChanged(parentActiveState);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Get the Scene that this Entity is in.
    //-----------------------------------------------------------------------------------------------------------------------------
    Scene* SceneEntity::GetScene() const
    {
        MCP_CHECK(m_pLayer);
        return m_pLayer->GetScene();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the Entities' layer.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SceneEntity::SetLayer(SceneLayer* pLayer)
    {
        // If this layer is not nullptr and the LayerIds don't match then return.
        if (pLayer && pLayer->GetLayerId() != GetLayerId())
        {
            MCP_WARN("SceneEntity", "Attempted to set an SceneEntity's layer to one that it doesn't belong to!");
            return;
        }

        m_pLayer = pLayer;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Get the Layer that this SceneEntity is in.
    //-----------------------------------------------------------------------------------------------------------------------------
    SceneLayer* SceneEntity::GetLayer() const
    {
        return m_pLayer;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get the construction data from an XML file.
    ///		@param element : The root element of the SceneEntity.
    //-----------------------------------------------------------------------------------------------------------------------------
    SceneEntityConstructionData SceneEntity::GetEntityConstructionData(const XMLElement element)
    {
        SceneEntityConstructionData data;

        // Start Active flag
        data.startActive = element.GetAttributeValue<bool>("startActive", true);

        // Tag
        data.tag = element.GetAttributeValue<const char*>("tag", nullptr);

        return data;
    }

}
