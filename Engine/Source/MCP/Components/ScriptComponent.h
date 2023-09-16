#pragma once
// ScriptComponent.h

#include "Component.h"
#include "MCP/Scene/IUpdateable.h"

namespace mcp
{
    class ScriptComponent : public Component, public IUpdateable
    {
        MCP_DEFINE_COMPONENT_ID(ScriptComponent)

    public:
        ScriptComponent(Object* pObject);
        virtual ~ScriptComponent() override;

        virtual bool Init() override;
        virtual void Update(const float deltaTime) override = 0;
        virtual void OnLoad() = 0;

        static bool AddFromData(const void*, Object*) { return false; }
    };
}
    