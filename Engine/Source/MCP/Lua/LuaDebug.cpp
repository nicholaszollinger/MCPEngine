// LuaDebug.cpp

#include "LuaDebug.h"

#include <cassert>
#include "LuaSource.h"
#include "MCP/Debug/Assert.h"

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
        MCP_CHECK(lua_type(pState, stackIndex) == luaType);
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
    ///		@brief : Asserts that a condition in a script is true.
    //-----------------------------------------------------------------------------------------------------------------------------
    int LuaCheck(lua_State* pState)
    {
        [[maybe_unused]] const bool condition = lua_toboolean(pState, -1);
        lua_pop(pState, 1);

        MCP_CHECK(condition);
        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Log a message using the MCP Logging system. Unlike the logging system in the engine, LuaLogs are
    ///             restricted to a single string.
    ///
    ///             \n LUA PARAMS: const char* ScriptName, const char* Message, 
    ///             \n NO RETURN
    //-----------------------------------------------------------------------------------------------------------------------------
    int LuaLog(lua_State* pState)
    {
        [[maybe_unused]] const char* category = lua_tostring(pState, -2);
        [[maybe_unused]] const char* msg = lua_tostring(pState, -1);

        MCP_LOG(category, msg);

        // Pop the parameter.
        lua_pop(pState, 2);

        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Log a Warning message using the MCP Logging system. Unlike the logging system in the engine, LuaLogs are
    ///             restricted to a single string. 
    ///
    ///             \n LUA PARAMS: const char* ScriptName, const char* Message, 
    ///             \n NO RETURN
    //-----------------------------------------------------------------------------------------------------------------------------
    int LuaLogWarning(lua_State* pState)
    {
        [[maybe_unused]] const char* category = lua_tostring(pState, -2);
        [[maybe_unused]] const char* msg = lua_tostring(pState, -1);
        
        MCP_WARN(category, msg);

        // Pop the parameter.
        lua_pop(pState, 2);

        return 0;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Log an Error message using the MCP Logging system. Unlike the logging system in the engine, LuaLogs are
    ///             restricted to a single string. 
    ///
    ///             \n LUA PARAMS: const char* ScriptName, const char* Message, 
    ///             \n NO RETURN
    //-----------------------------------------------------------------------------------------------------------------------------
    int LuaLogError(lua_State* pState)
    {
        [[maybe_unused]] const char* category = lua_tostring(pState, -2);
        [[maybe_unused]] const char* msg = lua_tostring(pState, -1);
        
        MCP_ERROR(category, msg);

        // Pop the parameter.
        lua_pop(pState, 2);

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

    static int OpenDebug(lua_State* pState)
    {
        static constexpr luaL_Reg kDebugFuncs[]
        {
            {"LOG", LuaLog}
             ,{"WARN", LuaLogWarning}
            , {"ERROR", LuaLogError}
            , {"CHECK", LuaCheck}
            ,{nullptr, nullptr}
        };

        luaL_newlib(pState, kDebugFuncs);
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
        static constexpr const char* kScriptPath = "../MCPEngine/Engine/Scripts/Core/Debug.lua";

        // Load the debug module, placing the debug lib table on the stack.
        luaL_requiref(pState, "MCP_DEBUG", &OpenDebug, 1);
        lua_pop(pState, 1);
    }
}