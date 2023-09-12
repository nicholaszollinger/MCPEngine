#pragma once
// LuaDebug.h

struct lua_State;

namespace mcp
{
    void RegisterDebugFunctions(lua_State* pState);

    // Lua Logs
    int LuaLog(lua_State* pState);
    int LuaLogWarning(lua_State* pState);
    int LuaLogError(lua_State* pState);

    // Lua Type Asserts/Checks
    int AssertIsLuaString(lua_State* pState);
    int AssertIsLuaNumber(lua_State* pState);
    int AssertIsLuaBoolean(lua_State* pState);
    int AssertIsLuaTable(lua_State* pState);
    int AssertIsLuaFunction(lua_State* pState);
    int AssertIsLuaUserData(lua_State* pState);
    
    int CheckIsLuaString(lua_State* pState);
    int CheckIsLuaNumber(lua_State* pState);
    int CheckIsLuaBoolean(lua_State* pState);
    int CheckIsLuaTable(lua_State* pState);
    int CheckIsLuaFunction(lua_State* pState);
    int CheckIsLuaUserData(lua_State* pState);

    // C++ Type Checks
    void AssertIsLuaType(lua_State* pState, const int luaType, const int stackIndex = -1);
    bool IsLuaType(lua_State* pState, const int luaType, const int stackIndex = -1);
}