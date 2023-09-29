// IRenderable.cpp

#include "IRenderable.h"

namespace mcp
{
    IRenderable::IRenderable(const RenderLayer layer, const int zOrder)
        : m_renderableId(s_idCounter++)
        , m_zOrder(zOrder)
        , m_renderLayer(layer) // TODO: Delete
    {
        //
    }
}