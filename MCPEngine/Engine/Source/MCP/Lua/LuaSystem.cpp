// LuaSystem.cpp

#include "LuaSystem.h"

//#pragma warning(push)
#pragma warning(disable : 4701)
#include <lua.hpp>
//#pragma warning(pop)

#include "MCP/Lua/LuaDebug.h"

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Initialize the Lua System. This creates a new lua state instance and registers the debug functions
    ///             available in the mcp engine.
    ///		@returns : True if successful, false otherwise.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool LuaSystem::Init()
    {
        m_pState = luaL_newstate();
        luaL_openlibs(m_pState);

        // Register Debug functions.
        RegisterDebugFunctions(m_pState);

        // TODO potentially: Open any necessary/default/general/utility LuaScript files.

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Close the lua system. If the stack is not empty (meaning things were not cleaned up correctly),
    ///             this will post an error.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::Close()
    {
#if defined(DEBUG_LUA) && DEBUG_LUA
        if (!IStackEmpty())
        {
            LogError("LUA: Stack was not cleaned up correctly!");
            DumpStack();
            PopStack();
        }
#endif

        lua_close(m_pState);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Load a lua script to be used in this lua system.
    ///		@param pFilepath : Path to the script on disk.
    ///		@returns : True if the file successfully loaded, otherwise false.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool LuaSystem::LoadScript(const char* pFilepath) const
    {
        if (luaL_dofile(m_pState, pFilepath) != 0)
        {
            LogError("Failed to load Lua Script! Filepath: %", pFilepath);
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get a global string value among loaded lua scripts.
    ///		@param varName : Name of the global variable you are trying to find.
    ///		@returns : The value of the string, or nullptr if it fails.
    //-----------------------------------------------------------------------------------------------------------------------------
    const char* LuaSystem::GetString(const char* varName) const
    {
        if (!lua_getglobal(m_pState, varName))
        {
            LogError("LUA: No global string found with name '%'", varName);
            return nullptr;
        }

        assert(lua_type(m_pState, -1) == LUA_TSTRING);

        return GetString();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the top element of the stack as a string value.
    //-----------------------------------------------------------------------------------------------------------------------------
    const char* LuaSystem::GetString() const
    {
        const char* value = lua_tostring(m_pState, -1);

        // Pop the global from the stack
        lua_pop(m_pState, 1);

        return value;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get a global boolean value among loaded lua scripts.
    ///		@param varName : Name of the global variable you are trying to find.
    ///		@returns : std::optional<bool> that either contains the value or does not.
    //-----------------------------------------------------------------------------------------------------------------------------
    std::optional<bool> LuaSystem::GetBoolean(const char* varName) const
    {
        if (!lua_getglobal(m_pState, varName))
        {
            LogError("LUA: No global string found with name '%'", varName);
            return {};
        }

        assert(lua_type(m_pState, -1) == LUA_TBOOLEAN);

        return GetBoolean();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the top element of the stack as a boolean value.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool LuaSystem::GetBoolean() const
    {
        const bool value = lua_toboolean(m_pState, -1);

        // Pop the global from the stack
        lua_pop(m_pState, 1);

        return value;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get a global integer value among loaded lua scripts.
    ///		@param varName : Name of the global variable you are trying to find.
    ///		@returns : std::optional<int64_t> that either contains the value or does not.
    //-----------------------------------------------------------------------------------------------------------------------------
    std::optional<int64_t> LuaSystem::GetInteger(const char* varName) const
    {
        if (!lua_getglobal(m_pState, varName))
        {
            LogError("LUA: No global integer found with name '%'", varName);
            return {};
        }

        assert(lua_type(m_pState, -1) == LUA_TNUMBER);

        return GetInteger();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the top element of the stack as a boolean value.
    //-----------------------------------------------------------------------------------------------------------------------------
    int64_t LuaSystem::GetInteger() const
    {
        const int64_t value = lua_tointeger(m_pState, -1);

        lua_pop(m_pState, 1);

        return value;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get a global double value among loaded lua scripts.
    ///		@param varName : Name of the global variable you are trying to find.
    ///		@returns : std::optional<double> that either contains the value or does not.
    //-----------------------------------------------------------------------------------------------------------------------------
    std::optional<double> LuaSystem::GetNumber(const char* varName) const
    {
        if (!lua_getglobal(m_pState, varName))
        {
            LogError("LUA: No global float found with name '%'", varName);
            return {};
        }

        assert(lua_type(m_pState, -1) == LUA_TNUMBER);

        return GetNumber();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the top element of the stack as a boolean value.
    //-----------------------------------------------------------------------------------------------------------------------------
    double LuaSystem::GetNumber() const
    {
        const double value = lua_tonumber(m_pState, -1);

        lua_pop(m_pState, 1);

        return value;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : NOT IMPLEMENTED YET. DON'T USE.
    ///		@tparam LuaType : 
    ///		@param globalName : 
    ///		@returns : 
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename LuaType>
    LuaType* LuaSystem::GetGlobal(const char* globalName)
    {
        // TODO: Not fully implemented yet. This is going to be for userData. I just don't know enough
        // about it yet.

        if (!lua_getglobal(m_pState, globalName))
        {
            LogError("LUA: No global found with name '%'", globalName);
            return nullptr;
        }

        assert(lua_type(m_pState, -1) == LUA_TUSERDATA);

        LuaType* pOutput = static_cast<LuaType*>(lua_touserdata(m_pState, -1));

        lua_pop(m_pState, 1);

        return pOutput;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set a global c-string variable among loaded lua scripts.
    ///		@param varName : Name of the variable you want to set.
    ///		@param value : Value you want to set it to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::SetString(const char* varName, const char* value) const
    {
#if defined(DEBUG_LUA) && DEBUG_LUA
        // Check to see if the variable exists.
        if (!lua_getglobal(m_pState, varName))
        {
            LogWarning("LUA: Tried to set string to global that doesn't exist! No global found with name '%'", varName);
            return;
        }

        // Check to see if the global is a string type.
        if (!IsLuaType(m_pState, LUA_TSTRING))
        {
            LogWarning("LUA: Setting string value to global of a different type. Global name: '%'", varName);
        }
            
        lua_pop(m_pState, 1);

#endif
        lua_pushstring(m_pState, value);
        lua_setglobal(m_pState, varName); // This pops the stack.
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set a global boolean variable among loaded lua scripts.
    ///		@param varName : Name of the variable you want to set.
    ///		@param value : Value you want to set it to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::SetBoolean(const char* varName, const bool value) const
    {
#if defined(DEBUG_LUA) && DEBUG_LUA
        // Check to see if the variable exists.
        if (!lua_getglobal(m_pState, varName))
        {
            LogWarning("LUA: Tried to set bool to global that doesn't exist! No global found with name '%'", varName);
            return;
        }

        // Check to see if the global is a boolean type.
        if (!IsLuaType(m_pState, LUA_TBOOLEAN))
        {
            LogWarning("LUA: Setting boolean value to global of a different type. Global name: '%'", varName);
        }
            
        lua_pop(m_pState, 1);

#endif
        lua_pushboolean(m_pState, value);
        lua_setglobal(m_pState, varName); // This pops the stack.
    }
    
    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Set a global Integer variable among loaded lua scripts.
    ///		@param varName : Name of the variable you want to set.
    ///		@param value : Value you want to set it to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::SetInteger(const char* varName, const int64_t& value) const
    {
#if defined(DEBUG_LUA) && DEBUG_LUA
        // Check to see if the variable exists.
        if (!lua_getglobal(m_pState, varName))
        {
            LogWarning("LUA: Tried to set integer to global that doesn't exist! No global found with name '%'", varName);
            return;
        }

        // Check to see if the global is a number type.
        if (!IsLuaType(m_pState, LUA_TNUMBER))
        {
            LogWarning("LUA: Setting integer value to global of a different type. Global name: '%'", varName);
        }
            
        lua_pop(m_pState, 1);

#endif
        lua_pushinteger(m_pState, value);
        lua_setglobal(m_pState, varName); // This pops the stack.
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Set a global Number (a 'double') variable among loaded lua scripts.
    ///		@param varName : Name of the variable you want to set.
    ///		@param value : Value you want to set it to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::SetNumber(const char* varName, const double value) const
    {
#if defined(DEBUG_LUA) && DEBUG_LUA
        // Check to see if the variable exists.
        if (!lua_getglobal(m_pState, varName))
        {
            LogWarning("LUA: Tried to set float to global that doesn't exist! No global found with name '%'", varName);
            return;
        }

        // Check to see if the global is a number type.
        if (!IsLuaType(m_pState, LUA_TNUMBER))
        {
            LogWarning("LUA: Setting float value to global of a different type. Global name: '%'", varName);
        }
            
        lua_pop(m_pState, 1);

#endif
        lua_pushnumber(m_pState, value);
        lua_setglobal(m_pState, varName); // This pops the stack.
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Push a global lua function onto the stack.
    ///		@param pFunctionName : Name of the function.
    ///		@returns : True if the push succeeds, false otherwise.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool LuaSystem::PushFunction(const char* pFunctionName) const
    {
        // Get the global function
        if (!lua_getglobal(m_pState, pFunctionName))
        {
            LogError("Failed to push function! No function with name '%' found!");
            return false;
        }

        // Check to see if it is a lua function.
        if (lua_isfunction(m_pState, -1) == 0)
        {
            LogError("Failed to push function '%'! Value was not a function-type", pFunctionName);
            lua_pop(m_pState, 1);
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Actually calls the lua function, popping it and its parameters off the stack.
    ///		@param paramCount : Number of parameters that are on the stack.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::CallFunctionImpl(const int paramCount) const
    {
        // TODO: Create an error function to pass in.
        lua_pcall(m_pState, paramCount, 0, 0);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Pushes an integer onto the stack.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::PushInteger(const int64_t& val) const
    {
        lua_pushinteger(m_pState, val);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Pushes a double onto the stack.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::PushFloat(const double val) const
    {
        lua_pushnumber(m_pState, val);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Pushes and c-string onto the stack.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::PushString(const char* val) const
    {
        lua_pushstring(m_pState, val);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Pushes a boolean onto the stack.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::PushBool(const bool val) const
    {
        lua_pushboolean(m_pState, val);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : 'Gets' the table by pushing it onto the stack.
    ///		@param tableName : Name of the table.
    ///		@returns : True if the table is on the stack, false otherwise.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool LuaSystem::GetTable(const char* tableName) const
    {
        if (!lua_getglobal(m_pState, tableName))
        {
            LogError("LUA: No global table found with name '%'", tableName);
            return false;
        }

        // Should I do a check here instead of an assert?
        assert(lua_type(m_pState, -1) == LUA_TTABLE);

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : 'Gets' the element by pushing the value of the table element onto the stack.
    ///		@param elementName : Name of the element you are trying to get.
    ///		@returns : True if successful, false if no element was found with that name.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool LuaSystem::GetElement(const char* elementName) const
    {
        // Push the name of the element that we want onto the stack.
        lua_pushstring(m_pState, elementName);

        if (!lua_gettable(m_pState, -2))
        {
            LogError("LUA: Failed to find element with name: %", elementName);
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Sets the element in the table. This assumes that we have pushed the table, value and the name of the
    ///             element onto the stack already. This will also pop the table from the stack.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::SetElement() const
    {
        lua_settable(m_pState, -3);

        // Pop the table from the stack.
        lua_pop(m_pState, 1);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Pop a number of values off the stack. If no value is passed in or count is set to zero, the entire stack will
    ///             be popped.
    ///		@param count : Number of values to pop. If you want to pop all of them, leave it as the default of zero.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::PopStack(const int count) const
    {
        // If we are passing a specific number in, pop that many values off the stack.
        if (count > 0)
        {
            lua_pop(m_pState, count);
        }

        // Otherwise, pop the entire contents of the stack.
        else
        {
            const int stackCount = lua_gettop(m_pState);
            lua_pop(m_pState, stackCount);
        }
    }

#if defined(DEBUG_LUA) && DEBUG_LUA

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Check to see if the stack in our internal state is empty or not.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool LuaSystem::IStackEmpty() const
    {
        if (lua_gettop(m_pState) != 0)
            return false;

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Check to see if the size (number of elements) of the stack matches the 'size' value.
    ///		@param size : Size you think the stack should be.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool LuaSystem::VerifyStackSize(const int size) const
    {
        if (lua_gettop(m_pState) != size)
            return false;

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Prints out the stack in the LuaState into the log window.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaSystem::DumpStack() const
    {
        if (IStackEmpty())
        {
            Log("Lua Stack Empty!");
            return;
        }

        const int stackSize = lua_gettop(m_pState);

        std::string stackString("Dumping Lua Stack...\n");
        
        for (int i = 1; i <= stackSize; ++i)
        {
            const int type = lua_type(m_pState, i);

            switch(type)
            {
                case LUA_TSTRING: 
                {
                    stackString += lua_tostring(m_pState, i);
                    stackString += "\n";
                    break;
                }

                case LUA_TNUMBER: 
                {
                    stackString += std::to_string(lua_tonumber(m_pState, i));
                    stackString += "\n";
                    break;
                }

                //case LUA_TTABLE:
                //{
                //    // TODO: Do I want to print out the table?
                //    Log
                //    break;  
                //}

                case LUA_TBOOLEAN:
                {
                    if (lua_toboolean(m_pState, i))
                    {
                        stackString += "true\n";
                    }

                    else
                        stackString += "false\n";

                    break;
                }

                // Default to just printing the type name.
                default:
                {
                    stackString += lua_typename(m_pState, type);
                    stackString += "\n";
                }
            }
        }

        Log("%", stackString);
    }

#endif
}