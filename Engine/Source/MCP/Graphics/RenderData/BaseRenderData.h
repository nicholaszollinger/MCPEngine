#pragma once
// BaseRenderData.h

#include "Utility/Types/Rect.h"

namespace mcp
{
    enum class RenderFlip2D
    {
        kNone,
        kHorizontal,
        kVertical
    };

    struct BaseRenderData
    {
        BaseRenderData() = default;
        virtual ~BaseRenderData() = default;

        RectF destinationRect   = {};
        RectInt crop              = {};
        Vec2 anglePivot         = {};
        double angle            = 0.0;
        RenderFlip2D flip       = RenderFlip2D::kNone;
    };
}