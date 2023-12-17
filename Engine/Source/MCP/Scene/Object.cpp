// Object.cpp

#include "Object.h"

#include "MCP/Scene/Scene.h"

namespace mcp
{
    Object::Object(const SceneEntityConstructionData& data)
        : SceneEntity(data)
    {
        //
    }

    Object::~Object()
    {
        // Delete the components in reverse order, due to component dependencies.
        for (size_t i = 0; i < m_components.size(); ++i)
        {
            auto* pComponent = m_components[m_components.size() - 1 - i];
            BLEACH_DELETE(pComponent);
            pComponent = nullptr;
        }
    }

    bool Object::Init()
    {
        for (auto* pComponent : m_components)
        {
            if(!pComponent->Init())
            {
                MCP_ERROR("Object", "Failed to initialize Object!");
                return false;
            }
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Initialization step when the scene has finished loading. Used to 
    ///		@returns : 
    //-----------------------------------------------------------------------------------------------------------------------------
    bool Object::PostLoadInit()
    {
        for (auto* pComponent : m_components)
        {
            if (!pComponent->PostLoadInit())
            {
                MCP_ERROR("Object", "Object failed post Init!");
                return false;
            }
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Add a previously created Component to this Object.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Object::AddComponent(Component* pComponent)
    {
        if (!pComponent)
        {
            MCP_WARN("Object", "Attempted to add Component* that was nullptr!");
            return;
        }

        // Set the owner directly.
        pComponent->m_pOwner = this;
        pComponent->Init();

        // If this object as a parent, we need to let the component respond.
        if (HasAParent())
        {
            pComponent->OnOwnerParentSet(GetParent());
        }

        // Set the Component to its starting active state.
        pComponent->OnOwnerActiveChanged(IsActive());

        // Add the Component to our list.
        m_components.emplace_back(pComponent);
    }

    Object* Object::GetChildByTag(const StringId tag)
    {
        auto* pChild = SceneEntity::GetChildByTag(tag);
        if (!pChild)
            return nullptr;

        return SafeCastEntity<Object>(pChild);
    }

    Object* Object::GetChildByTag(const StringId tag) const
    {
        auto* pChild = SceneEntity::GetChildByTag(tag);
        if (!pChild)
            return nullptr;

        return SafeCastEntity<Object>(pChild);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Get the World Layer that this Object is in.
    //-----------------------------------------------------------------------------------------------------------------------------
    WorldLayer* Object::GetWorld() const
    {
        return SceneLayer::SafeCastLayer<WorldLayer>(GetLayer());
    }

    void Object::OnActive()
    {
        // Update our component's active state
        for (auto* pComponent : m_components)
        {
            pComponent->OnOwnerActiveChanged(true);
        }
    }

    void Object::OnInactive()
    {
        // Update our component's active state
        for (auto* pComponent : m_components)
        {
            pComponent->OnOwnerActiveChanged(false);
        }
    }

    void Object::OnDestroy()
    {
        // Signal to the components that we are being destroyed.
        for (auto* pComponent : m_components)
        {
            pComponent->OnDestroy();
        }
    }

    void Object::OnParentSet()
    {
        for (auto* pComponent : m_components)
        {
            pComponent->OnOwnerParentSet(static_cast<Object*>(GetParent()));
        }
    }
}
