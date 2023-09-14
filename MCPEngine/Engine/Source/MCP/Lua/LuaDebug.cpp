// LuaDebug.cpp

#include "LuaDebug.h"

#include <cassert>
#include "LuaSource.h"

#include "MCP/Debug/Log.h"

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Assert that the value at the stackIndex is of a certain LuaType. Note, this calls
    ///             assert, so this does nothing in release.
    ///		@param pState : State we are evaluating.
    ///		@param luaType : Type that we are checking. Example: LUA_TSTRING
    ///		@param stackIndex : Position of the value in the stack. Default is -1.
    //-----------------------------------------------------------------------------------------------------------------------------
    void AssertIsLuaType([[maybe_unused]] lua_State* pState, [[maybe_unused]] const int luaType, [[maybe_unused]] const int stackIndex)
    {
        assert(lua_type(pState, stackIndex) == luaType);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Check if the value at the stackIndex is of a certain LuaType.
    ///		@param pState : State we are evaluating.
    ///		@param luaType : Type that we are checking. Example: LUA_TSTRING
    ///		@param stackIndex : Position of the value in the stack. Default is -1.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool IsLuaType(lua_State* pState, const int luaType, const int stackIndex)
    {
        return lua_type(pState, stackIndex) == luaType;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Log a message using the MCP Logging system. Unlike the logging system in the engine, LuaLogs are
    ///             restricted to a single string.
    ///
    ///             \n LUA PARAMS: const char* Message
    ///             \n NO RETURN
    //-----------------------------------------------------------------------------------------------------------------------------
    int LuaLog(lua_State* pState)
    {
        [[maybe_unused]] const char* msg = lua_tostring(pState, -1);

        // TODO: This log category needs to be changed to know what the lua script is. I need to add another param.
        MCP_LOG("Script", msg);

        // Pop the parameter.
        lua_pop(pState, 1);

        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Log a Warning message using the MCP Logging system. Unlike the logging system in the engine, LuaLogs are
    ///             restricted to a single string. 
    ///
    ///             \n LUA PARAMS: const char* Message
    ///             \n NO RETURN
    //-----------------------------------------------------------------------------------------------------------------------------
    int LuaLogWarning(lua_State* pState)
    {
        [[maybe_unused]] const char* msg = lua_tostring(pState, -1);

        // TODO: I need to be able to add the script name to this warning message.
        MCP_WARN("Script", msg);

        // Pop the parameter.
        lua_pop(pState, 1);

        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Log an Error message using the MCP Logging system. Unlike the logging system in the engine, LuaLogs are
    ///             restricted to a single string. 
    ///
    ///             \n LUA PARAMS: const char* Message
    ///             \n NO RETURN
    //-----------------------------------------------------------------------------------------------------------------------------
    int LuaLogError(lua_State* pState)
    {
        [[maybe_unused]] const char* msg = lua_tostring(pState, -1);

        // TODO: I need to be able to add the script name to this warning message.
        MCP_ERROR("Script", msg);

        // Pop the parameter.
        lua_pop(pState, 1);

        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Assert the value at the top of the stack is a lua string.
    ///
    ///             \n NO PARAMS
    ///             \n NO RETURN
    //-----------------------------------------------------------------------------------------------------------------------------
    int AssertIsLuaString(lua_State* pState)
    {
        AssertIsLuaType(pState, LUA_TSTRING);
        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Assert the value at the top of the stack is a lua number.
    ///
    ///             \n NO PARAMS
    ///             \n NO RETURN
    //-----------------------------------------------------------------------------------------------------------------------------
    int AssertIsLuaNumber(lua_State* pState)
    {
        AssertIsLuaType(pState, LUA_TNUMBER);
        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Assert the value at the top of the stack is a lua boolean.
    ///
    ///             \n NO PARAMS
    ///             \n NO RETURN
    //-----------------------------------------------------------------------------------------------------------------------------
    int AssertIsLuaBoolean(lua_State* pState)
    {
        AssertIsLuaType(pState, LUA_TBOOLEAN);
        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Assert the value at the top of the stack is a lua table.
    ///
    ///             \n NO PARAMS
    ///             \n NO RETURN
    //-----------------------------------------------------------------------------------------------------------------------------
    int AssertIsLuaTable(lua_State* pState)
    {
        AssertIsLuaType(pState, LUA_TTABLE);
        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Assert the value at the top of the stack is a lua function.
    ///
    ///             \n NO PARAMS
    ///             \n NO RETURN
    //-----------------------------------------------------------------------------------------------------------------------------
    int AssertIsLuaFunction(lua_State* pState)
    {
        AssertIsLuaType(pState, LUA_TFUNCTION);
        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Assert the value at the top of the stack is a lua user data type.
    ///
    ///             \n NO PARAMS
    ///             \n NO RETURN
    //-----------------------------------------------------------------------------------------------------------------------------
    int AssertIsLuaUserData(lua_State* pState)
    {
        AssertIsLuaType(pState, LUA_TUSERDATA);
        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Check to see if the value at the top of the stack is a lua string.
    ///
    ///             \n NO PARAMS
    ///             \n RETURNS: Boolean based on the result.
    //-----------------------------------------------------------------------------------------------------------------------------
    int CheckIsLuaString(lua_State* pState)
    {
        const bool isType = IsLuaType(pState, LUA_TSTRING);
        lua_pushboolean(pState, isType);

        return 1;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Check to see if the value at the top of the stack is a lua string.
    ///
    ///             \n NO PARAMS
    ///             \n RETURNS: Boolean based on the result.
    //-----------------------------------------------------------------------------------------------------------------------------
    int CheckIsLuaNumber(lua_State* pState)
    {
        const bool isType = IsLuaType(pState, LUA_TNUMBER);
        lua_pushboolean(pState, isType);

        return 1;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Check to see if the value at the top of the stack is a lua Boolean.
    ///
    ///             \n NO PARAMS
    ///             \n RETURNS: Boolean based on the result.
    //-----------------------------------------------------------------------------------------------------------------------------
    int CheckIsLuaBoolean(lua_State* pState)
    {
        const bool isType = IsLuaType(pState, LUA_TBOOLEAN);
        lua_pushboolean(pState, isType);

        return 1;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Check to see if the value at the top of the stack is a lua Table.
    ///
    ///             \n NO PARAMS
    ///             \n RETURNS: Boolean based on the result.
    //-----------------------------------------------------------------------------------------------------------------------------
    int CheckIsLuaTable(lua_State* pState)
    {
        const bool isType = IsLuaType(pState, LUA_TTABLE);
        lua_pushboolean(pState, isType);

        return 1;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Check to see if the value at the top of the stack is a lua Function.
    ///
    ///             \n NO PARAMS
    ///             \n RETURNS: Boolean based on the result.
    //-----------------------------------------------------------------------------------------------------------------------------
    int CheckIsLuaFunction(lua_State* pState)
    {
        const bool isType = IsLuaType(pState, LUA_TFUNCTION);
        lua_pushboolean(pState, isType);

        return 1;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Check to see if the value at the top of the stack is a lua user data.
    ///
    ///             \n NO PARAMS
    ///             \n RETURNS: Boolean based on the result.
    //-----------------------------------------------------------------------------------------------------------------------------
    int CheckIsLuaUserData(lua_State* pState)
    {
        const bool isType = IsLuaType(pState, LUA_TUSERDATA);
        lua_pushboolean(pState, isType);

        return 1;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Registers the debug functions to the Lua State. This includes Logging, Type Asserts and Type checks.
    ///		@param pState : State we are registering to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void RegisterDebugFunctions(lua_State* pState)
    {
        // Register Log Functions.
        lua_pushcfunction(pState, &LuaLog);
        lua_setglobal(pState, "Log");

        lua_pushcfunction(pState, &LuaLogWarning);
        lua_setglobal(pState, "LogWarning");

        lua_pushcfunction(pState, &LuaLogError);
        lua_setglobal(pState, "LogError");

        // Register Type Asserts:
        lua_pushcfunction(pState, &AssertIsLuaString);
        lua_setglobal(pState, "AssertIsString");

        lua_pushcfunction(pState, &AssertIsLuaBoolean);
        lua_setglobal(pState, "AssertIsBoolean");

        lua_pushcfunction(pState, &AssertIsLuaNumber);
        lua_setglobal(pState, "AssertIsNumber");

        lua_pushcfunction(pState, &AssertIsLuaFunction);
        lua_setglobal(pState, "AssertIsFunction");

        lua_pushcfunction(pState, &AssertIsLuaTable);
        lua_setglobal(pState, "AssertIsTable");

        lua_pushcfunction(pState, &AssertIsLuaUserData);
        lua_setglobal(pState, "AssertIsUserData");

        // Register Type Checks
        lua_pushcfunction(pState, &CheckIsLuaString);
        lua_setglobal(pState, "CheckIsString");

        lua_pushcfunction(pState, &CheckIsLuaNumber);
        lua_setglobal(pState, "CheckIsNumber");

        lua_pushcfunction(pState, &CheckIsLuaBoolean);
        lua_setglobal(pState, "CheckIsBoolean");

        lua_pushcfunction(pState, &CheckIsLuaTable);
        lua_setglobal(pState, "CheckIsTable");

        lua_pushcfunction(pState, &CheckIsLuaFunction);
        lua_setglobal(pState, "CheckIsFunction");

        lua_pushcfunction(pState, &CheckIsLuaUserData);
        lua_setglobal(pState, "CheckIsUserData");
    }
}