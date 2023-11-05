#pragma once
// Layout.h

#include "AlignmentTypes.h"

#define MCP_DEBUG_RENDER_LAYOUT_BOUNDS 0

#ifndef _DEBUG
#define MCP_DEBUG_RENDER_LAYOUT_BOUNDS 0
#endif

namespace mcp
{
    class XMLElement;

    struct LayoutData
    {
        HorizontalAlignment horizontal = HorizontalAlignment::kCenter;
        VerticalAlignment vertical = VerticalAlignment::kCenter;
        float padding = 20.f;
    };

    LayoutData GetLayoutData(const XMLElement element);
}