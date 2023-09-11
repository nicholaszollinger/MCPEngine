// Lua.cpp

#include "Lua.h"

namespace mcp::lua
{
    //-----------------------------------------------------------------------------------------------------------------------------
    ///     @brief : Initializes the internal LuaSystem.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool LuaLayer::Init()
    {
        return m_system.Init();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Closes the internal LuaSystem.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LuaLayer::Close()
    {
        m_system.Close();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Load a Lua script to be used in the engine-wide system.
    ///		@param pFilepath : Path to the Lua script file in memory.
    ///		@returns : True if the script was opened successfully, false otherwise.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool LoadScript(const char* pFilepath)
    {
        return LuaLayer::Get()->GetSystem().LoadScript(pFilepath);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Consider making this a std::nullopt type to handle cases where we couldn't find the global.
    //		
    ///		@brief : Get a global boolean value among loaded lua scripts.
    ///		@param varName : Name of the global variable you are trying to find.
    ///		@returns : The value of the boolean. NOTE: If this function fails, this will just return false!
    //-----------------------------------------------------------------------------------------------------------------------------
    std::optional<bool> GetBoolean(const char* varName)
    {
        return LuaLayer::Get()->GetSystem().GetBoolean(varName);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Consider making this a std::nullopt type to handle cases where we couldn't find the global.
    //		
    ///		@brief : Get a global Integer value among loaded lua scripts.
    ///		@param varName : Name of the global variable you are trying to find.
    ///		@returns : The value of the integer. NOTE: If this function fails, this will just return 0!
    //-----------------------------------------------------------------------------------------------------------------------------
    std::optional<int64_t> GetInteger(const char* varName)
    {
        return LuaLayer::Get()->GetSystem().GetInteger(varName);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Consider making this a std::nullopt type to handle cases where we couldn't find the global.
    //		
    ///		@brief : Get a global double value among loaded lua scripts.
    ///		@param varName : Name of the global variable you are trying to find.
    ///		@returns : The value of the double. NOTE: If this function fails, this will just return 0.0!
    //-----------------------------------------------------------------------------------------------------------------------------
    std::optional<double> GetNumber(const char* varName)
    {
        return LuaLayer::Get()->GetSystem().GetNumber(varName);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      Consider making this a std::nullopt type to handle cases where we couldn't find the global.
    //		
    ///		@brief : Get a global c-string value among loaded lua scripts.
    ///		@param varName : Name of the global variable you are trying to find.
    ///		@returns : The value of the string. NOTE: If this function fails, this will just return an empty string!
    //-----------------------------------------------------------------------------------------------------------------------------
    const char* GetString(const char* varName)
    {
        return LuaLayer::Get()->GetSystem().GetString(varName);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set a global boolean variable among loaded lua scripts.
    ///		@param varName : Name of the variable you want to set.
    ///		@param value : Value you want to set it to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SetBoolean(const char* varName, const bool value)
    {
        LuaLayer::Get()->GetSystem().SetBoolean(varName, value);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set a global Integer variable among loaded lua scripts.
    ///		@param varName : Name of the variable you want to set.
    ///		@param value : Value you want to set it to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SetInteger(const char* varName, const int64_t& value)
    {
        LuaLayer::Get()->GetSystem().SetInteger(varName, value);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set a global Number (a 'double') variable among loaded lua scripts.
    ///		@param varName : Name of the variable you want to set.
    ///		@param value : Value you want to set it to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SetNumber(const char* varName, const double value)
    {
        LuaLayer::Get()->GetSystem().SetNumber(varName, value);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set a global c-string variable among loaded lua scripts.
    ///		@param varName : Name of the variable you want to set.
    ///		@param value : Value you want to set it to.
    //-----------------------------------------------------------------------------------------------------------------------------
    void SetString(const char* varName, const char* value)
    {
        LuaLayer::Get()->GetSystem().SetString(varName, value);
    }
}