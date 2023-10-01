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
        auto* pWorld = m_pOwner->GetWorld();
        assert(pWorld);
        pWorld->RemoveUpdateable(this);
    }

    bool ScriptComponent::Init()
    {
        // Add this component to the list of updateables in the scene.
        auto* pWorld = m_pOwner->GetWorld();
        assert(pWorld);
        pWorld->AddUpdateable(this);

        return true;
    }
}