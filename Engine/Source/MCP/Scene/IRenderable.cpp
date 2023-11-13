// IRenderable.cpp

#include "IRenderable.h"

namespace mcp
{
    IRenderable::IRenderable(const RenderLayer layer, const int zOrder)
        : m_pRenderableParent(nullptr)
        , m_renderableId(s_idCounter++)
        , m_zOrder(zOrder)
        , m_renderLayer(layer) // TODO: Delete
    {
        //
    }

    int IRenderable::GetZOrder() const
    {
        if (m_pRenderableParent)
        {
            return m_pRenderableParent->GetZOrder() + 1;
        }

        return m_zOrder;
    }

}