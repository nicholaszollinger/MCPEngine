// PrimitiveComponent.cpp

#include "PrimitiveComponent.h"
#include "TransformComponent.h"
#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    PrimitiveComponent::PrimitiveComponent(Object* pObject, const RenderLayer layer, const int zOrder)
        : Component(pObject)
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
        auto* pWorld = m_pOwner->GetWorld();
        assert(pWorld);
        pWorld->RemoveRenderable(this);
    }
    
    bool PrimitiveComponent::Init()
    {
        // Add the renderable to the scene.
        auto* pWorld = m_pOwner->GetWorld();
        assert(pWorld);
        pWorld->AddRenderable(this);

        // TransformComponent is required.
        m_pTransformComponent = m_pOwner->GetComponent<mcp::TransformComponent>();
        assert(m_pTransformComponent);

        return true;
    }
}