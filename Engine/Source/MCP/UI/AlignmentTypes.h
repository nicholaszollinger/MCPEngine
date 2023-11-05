#pragma once
// AlignmentTypes.h

#include "Utility/Generic/Hash.h"

namespace mcp
{
    enum class HorizontalAlignment : uint32_t
    {
        kLeft = HashString32("left")
        , kCenter = HashString32("center")
        , kRight = HashString32("right")
    };

    enum class VerticalAlignment : uint32_t
    {
        kTop = HashString32("top")
        , kCenter = HorizontalAlignment::kCenter
        , kBottom = HashString32("bottom")
    };

}
