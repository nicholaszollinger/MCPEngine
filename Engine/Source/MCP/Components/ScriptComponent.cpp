// ScriptComponent.cpp
#include "ScriptComponent.h"

#include <cassert>
#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    ScriptComponent::ScriptComponent(Object* pObject)
        : Component(pObject)
    {
        //
    }

    ScriptComponent::~ScriptComponent()
    {
        auto* pScene = m_pOwner->GetScene();
        assert(pScene);
        pScene->RemoveUpdateable(m_updateableId);
    }

    bool ScriptComponent::Init()
    {
        // Add this component to the list of updateables in the scene.
        auto* pScene = m_pOwner->GetScene();
        assert(pScene);
        pScene->AddUpdateable(m_updateableId, this);

        return true;
    }
}