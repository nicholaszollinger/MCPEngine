// PrimitiveComponent.cpp

#include "PrimitiveComponent.h"
#include "TransformComponent.h"
#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    PrimitiveComponent::PrimitiveComponent(const RenderLayer layer, const int zOrder)
        : Component(true)
        , IRenderable(layer, zOrder)
        , m_pTransformComponent(nullptr)
        , m_color(kDefaultColor)
        , m_renderType(RenderType::kFill)
    {
        //
    }

    PrimitiveComponent::~PrimitiveComponent()
    {
        // Remove the renderable from the scene.
        auto* pWorld = GetOwner()->GetWorld();
        MCP_CHECK(pWorld);
        pWorld->RemoveRenderable(this);
    }
    
    bool PrimitiveComponent::Init()
    {
        // Add the renderable to the scene.
        auto* pWorld = GetOwner()->GetWorld();
        MCP_CHECK(pWorld);
        pWorld->AddRenderable(this);

        // TransformComponent is required.
        m_pTransformComponent = GetOwner()->GetComponent<mcp::TransformComponent>();
        assert(m_pTransformComponent);

        return true;
    }

    void PrimitiveComponent::OnActive()
    {
        GetOwner()->GetWorld()->AddRenderable(this);
    }

    void PrimitiveComponent::OnInactive()
    {
        GetOwner()->GetWorld()->RemoveRenderable(this);
    }
}