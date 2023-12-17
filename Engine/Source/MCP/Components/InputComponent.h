#pragma once
// InputComponent.h
// TODO: I didn't have time to complete the InputSystem the way I wanted.

#include "Component.h"
#include "MCP/Input/InputCodes.h"
#include "MCP/Input/InputListener.h"

namespace mcp
{
    class InputComponent final : public Component, public InputListener
    {
        friend class WorldLayer;
    public:
        using InputActionCallback = std::function<void(const InputActionValue&)>;
#define MCP_BIND_MEMBER_FUNC(func) \
        [this](const mcp::InputActionValue& value) { this->func(value); }

    private:
        MCP_DEFINE_COMPONENT_ID(InputComponent)
        //std::unordered_map<InputControl, InputAction*> m_controls;

    public:
        InputComponent();

        void AddAction(InputAction* action);
        void BindAction(const InputAction* action, InputTriggerEvent event, void* pOwner, const InputActionCallback& memberFunction);
        bool HasActionForControl(const MCPKey key);

        static InputComponent* AddFromData(const XMLElement element);

    private:
        virtual void OnActive() override;
        virtual void OnInactive() override;
        void TickInput(const float deltaTimeMs);
    };
}