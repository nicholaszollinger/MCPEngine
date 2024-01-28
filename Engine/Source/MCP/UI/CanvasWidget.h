#pragma once
// CanvasWidget.h

#include "Widget.h"

namespace mcp
{
    class CanvasWidget final : public Widget
    {
    private:
        MCP_DEFINE_WIDGET(CanvasWidget)

    public:
        CanvasWidget(const WidgetConstructionData& data);
        static CanvasWidget* AddFromData(const XMLElement element);

        static void RegisterLuaFunctions(lua_State* pState);
    };
}