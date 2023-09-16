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
        auto* pScene = m_pOwner->GetScene();
        assert(pScene);
        pScene->RemoveRenderable(this);
    }
    
    bool PrimitiveComponent::Init()
    {
        // Add the renderable to the scene.
        auto* pScene = m_pOwner->GetScene();
        assert(pScene);
        pScene->AddRenderable(this);

        // TransformComponent is required.
        m_pTransformComponent = m_pOwner->GetComponent<mcp::TransformComponent>();
        assert(m_pTransformComponent);

        return true;
    }
}