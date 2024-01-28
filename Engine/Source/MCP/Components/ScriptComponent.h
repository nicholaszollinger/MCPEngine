#pragma once
// ScriptComponent.h

#include "Component.h"
#include "MCP/Scene/IUpdateable.h"

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : [NOT IMPLEMENTED YET]. This is going to be my 'Monobehavior' class.
    //-----------------------------------------------------------------------------------------------------------------------------
    class ScriptComponent : public Component, public IUpdateable
    {
        MCP_DEFINE_COMPONENT_ID(ScriptComponent)

    public:
        ScriptComponent();
        virtual ~ScriptComponent() override;

        virtual bool Init() override;
        virtual void Update(const float deltaTime) override = 0;
        virtual void OnLoad() = 0;
        
        static ScriptComponent* AddFromData(const XMLElement) { return nullptr; }
    };
}
    