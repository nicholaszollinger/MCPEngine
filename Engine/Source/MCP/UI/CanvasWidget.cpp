// CanvasWidget.cpp

#include "CanvasWidget.h"

namespace mcp
{
    CanvasWidget::CanvasWidget(const WidgetConstructionData& data)
        : Widget(data)
    {
        m_isInteractable = false;
    }

    CanvasWidget* CanvasWidget::AddFromData(const XMLElement element)
    {
        RectF rect;
        rect.x = element.GetAttribute<float>("x");
        rect.y = element.GetAttribute<float>("y");
        rect.width = element.GetAttribute<float>("width");
        rect.height = element.GetAttribute<float>("height");

        WidgetConstructionData data;
        data.rect = rect;
        data.anchor.x = element.GetAttribute<float>("anchorX", 0.5f);
        data.anchor.y = element.GetAttribute<float>("anchorY", 0.5f);
        data.isInteractable = false;
        data.zOffset = element.GetAttribute<int>("zOffset", 1);

        return BLEACH_NEW(CanvasWidget(data));
    }
}