// ApplicationEvent.cpp

#include "ApplicationEvent.h"

namespace mcp
{
    static constexpr bool IsAlpha(const uint32_t val)
    {
        return val >= 'A' && val <= 'Z' || val >= 'a' && val <= 'z';
    }

    static constexpr bool IsNumeric(const uint32_t val)
    {
        return val >= '0' && val <= '9';
    }

    uint32_t KeyEvent::GetGlyphValue() const
    {
        const auto glyph = static_cast<uint32_t>(m_keycode);

        if (IsAlpha(glyph))
        {
            if (m_shift)
            {
                return glyph - ('a' - 'A');
            }
            
            return glyph;
        }

        return glyph;
    }
}
