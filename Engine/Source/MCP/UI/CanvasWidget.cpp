// CanvasWidget.cpp

#include "CanvasWidget.h"
#include "LuaSource.h"
#include "MCP/Scene/UILayer.h"

namespace mcp
{
    CanvasWidget::CanvasWidget(const WidgetConstructionData& data)
        : Widget(data)
    {
        m_isInteractable = false;
    }

    CanvasWidget* CanvasWidget::AddFromData(const XMLElement element)
    {
        const auto data = GetWidgetConstructionData(element);

        return BLEACH_NEW(CanvasWidget(data));
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Attempts to find a child CanvasWidget.
    ///
    ///     \n LUA PARAMS: Widget* pWidget, const char* ChildWidgetTag
    ///     \n RETURNS: userData pointer to the found Canvas Widget, or nullptr in the case of a fail.
    //-----------------------------------------------------------------------------------------------------------------------------
    static int GetChildCanvasWidgetByTag(lua_State* pState)
    {
        //MCP_LOG("Lua", "Calling GetChildCanvasWidgetByTag() from lua...");

        // Get the Widget and string parameters off the stack.
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);
        const auto* pTag = lua_tostring(pState, -1);
        MCP_CHECK(pTag);

        // Pop the two parameters
        lua_pop(pState, 2);

        // Try to find the result.
        auto* pResult = pWidget->FindChildByTag<CanvasWidget>(pTag);

        // If valid, return a the data as a pointer.
        if (pResult)
            lua_pushlightuserdata(pState, pResult);
        // Otherwise, return nil.
        else
            lua_pushnil(pState);

        return 1;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Attempts to find a CanvasWidget in the UILayer.
    ///
    ///     \n LUA PARAMS: Widget* pWidget, const char* WidgetTag
    ///     \n RETURNS: userData pointer to the found Canvas Widget, or nullptr in the case of a fail.
    //-----------------------------------------------------------------------------------------------------------------------------
    static int GetCanvasWidgetByTag(lua_State* pState)
    {
        //MCP_LOG("Lua", "Calling GetChildCanvasWidgetByTag() from lua...");

        // Get the Widget and string parameters off the stack.
        auto* pWidget = static_cast<Widget*>(lua_touserdata(pState, -2));
        MCP_CHECK(pWidget);
        const auto* pTag = lua_tostring(pState, -1);
        MCP_CHECK(pTag);

        // Pop the two parameters
        lua_pop(pState, 2);

        // Try to find the result in the UILayer.
        auto* pResult = pWidget->GetUILayer()->GetWidgetByTag<CanvasWidget>(pTag);

        // If valid, return a the data as a pointer.
        if (pResult)
            lua_pushlightuserdata(pState, pResult);
        // Otherwise, return nil.
        else
            lua_pushnil(pState);

        return 1;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Register lua functions specific to the Canvas Widget.
    ///		@param pState : 
    //-----------------------------------------------------------------------------------------------------------------------------
    void CanvasWidget::RegisterLuaFunctions(lua_State* pState)
    {
        static constexpr luaL_Reg kFuncs[]
        {
             {"GetChildCanvasWidgetByTag", &GetChildCanvasWidgetByTag}
             ,{"GetCanvasWidget", &GetCanvasWidgetByTag}
            ,{nullptr, nullptr}
        };

        lua_getglobal(pState, "Widget");
        MCP_CHECK(lua_type(pState, -1) == LUA_TTABLE);

        // Set its functions
        luaL_setfuncs(pState, kFuncs, 0);

        // Pop the table off the stack.
        lua_pop(pState, 1);
    }
}
