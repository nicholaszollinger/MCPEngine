// Layout.cpp

#include "Layout.h"

#include "MCP/Core/Resource/Parsers/XMLParser.h"

namespace mcp
{
    LayoutData GetLayoutData(const XMLElement element)
    {
        LayoutData data;
        // Alignment
        const auto alignment = element.GetChildElement("Alignment");
        if (alignment.IsValid())
        {
            // Horizontal
            const char* horizontalTag = alignment.GetAttributeValue<const char*>("horizontal", "center");
            data.horizontal = static_cast<HorizontalAlignment>(HashString32(horizontalTag));

            // Vertical
            const char* verticalTag = alignment.GetAttributeValue<const char*>("vertical", "center");
            data.vertical = static_cast<VerticalAlignment>(HashString32(verticalTag));
        }

        // Padding
        const auto paddingElement = element.GetChildElement("Padding");
        if (paddingElement.IsValid())
        {
            data.padding = paddingElement.GetAttributeValue<float>("value", 20.f);
        }

        return data;
    }
}