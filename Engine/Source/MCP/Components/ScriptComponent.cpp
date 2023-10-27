// ScriptComponent.cpp
#include "ScriptComponent.h"

#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    ScriptComponent::ScriptComponent()
        : Component(true)
    {
        //
    }

    ScriptComponent::~ScriptComponent()
    {
        if (IsActive())
        {
            auto* pWorld = GetOwner()->GetWorld();
            MCP_CHECK(pWorld);
            pWorld->RemoveUpdateable(this);
        }
    }

    bool ScriptComponent::Init()
    {
        // Add this component to the list of updateables in the scene.
        auto* pWorld = GetOwner()->GetWorld();
        MCP_CHECK(pWorld);
        pWorld->AddUpdateable(this);

        return true;
    }
}