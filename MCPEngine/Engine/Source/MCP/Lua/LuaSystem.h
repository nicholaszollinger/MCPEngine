#pragma once
// LuaSystem.h

#include <optional>
#include "MCP/Application/Debug/Log.h"

#ifdef _DEBUG
    #define DEBUG_LUA 1
#endif

struct lua_State;

namespace mcp
{
    class LuaSystem
    {
        lua_State* m_pState = nullptr;

    public:
        // Initialization
        bool Init();
        void Close();

        // Load Lua Scripts
        bool LoadScript(const char* pFilepath) const;

        // Get Global Variables
        std::optional<bool> GetBoolean(const char* varName) const;
        std::optional<int64_t> GetInteger(const char* varName) const;
        std::optional<double> GetNumber(const char* varName) const;
        const char* GetString(const char* varName) const;
        template<typename LuaType> LuaType* GetGlobal(const char* globalName);

        // Set Global Variables
        void SetString(const char* varName, const char* value) const;
        void SetBoolean(const char* varName, const bool value) const;
        void SetInteger(const char* varName, const int64_t& value) const;
        void SetNumber(const char* varName, const double value) const;

        // Accessing Tables
        template<typename Type>
        std::optional<Type> GetElementInTable(const char* tableName, const char* elementName) const;

        template<typename Type>
        void SetElementInTable(const char* tableName, const char* elementName, Type&& val) const;
        
        // Calling Functions
        template<typename...Args>
        void CallFunction(const char* pFunctionName, Args&&...params) const;

    private:
        // Lua Function Call Helpers
        void CallFunctionImpl(const int paramCount) const;
        bool PushFunction(const char* pFunctionName) const;
        void PushInteger(const int64_t& val) const;
        void PushFloat(const double val) const;
        void PushString(const char* val) const;
        void PushBool(const bool val) const;
        [[nodiscard]] bool PushParams([[maybe_unused]] const int& paramCount) const { return true; }
        template<typename Param, typename...Args> bool PushParams(int& paramCount, Param&& param, Args&&...otherParams) const;
        template<typename Type> bool Push(Type&& val) const;

        // Lua Table Helpers
        [[maybe_unused]] bool GetTable(const char* tableName) const;
        bool GetElement(const char* elementName) const;
        void SetElement() const;

        // General Helpers
        void PopStack(const int count = 0) const;
        [[nodiscard]] bool GetBoolean() const;
        [[nodiscard]] int64_t GetInteger() const;
        [[nodiscard]] double GetNumber() const;
        [[nodiscard]] const char* GetString() const;

        // Debug functions.
#if defined(DEBUG_LUA) && DEBUG_LUA
        [[nodiscard]] bool IStackEmpty() const;
        [[nodiscard]] bool VerifyStackSize(const int size) const;
        void DumpStack() const;

#else
        bool IsStackEmpty() { return true; }
        bool VerifyStackSize([[maybe_unused]] const int size) { return true; }
        void DumpStack() {}
#endif
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    // LUA SYSTEM TEMPLATE DEFINITIONS
    //-----------------------------------------------------------------------------------------------------------------------------

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Call a lua function from among loaded lua scripts.
    ///		@tparam ParamTypes : Types of the parameters.
    ///		@param pFunctionName : Name of the lua function you are trying to call.
    ///		@param params : Any parameters that need to be passed to the function. NOTE: This can be empty!
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename... ParamTypes>
    void LuaSystem::CallFunction(const char* pFunctionName, ParamTypes&&... params) const
    {
        if (!PushFunction(pFunctionName))
        {
            return;
        }

        int paramCount = 0;

        // Push the arguments onto the LuaStack
        if (!PushParams(paramCount, params...))
        {
            // Failed to call the function!
            LogError("LUA: Failed to call lua function! Parameters were invalid!");
            PopStack(1);
            return;
        }

        // Call the function.
        CallFunctionImpl(paramCount);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get an element value in a table.
    ///		@tparam Type : Type of the element we are trying to get.
    ///		@param tableName : Name of the table we are trying to access.
    ///		@param elementName : Name of the element we are trying to get.
    ///		@returns : std::optional<Type> that either has value or does not.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename Type>
    std::optional<Type> LuaSystem::GetElementInTable(const char* tableName, const char* elementName) const
    {
        // If the table does not exist,
        if (!GetTable(tableName))
        {
            LogError("LUA: Failed to get element in Lua table named '%'!", tableName);
            return {};
        }

        // If the element does not exist.
        if (!GetElement(elementName))
        {
            LogError("LUA: Failed to get element in Lua table named '%'!", tableName);
            PopStack(1);
            return {};
        }

        // Return the correct type.
        using BaseType = std::remove_cv_t<Type>;
        using NonRefType = std::remove_reference_t<BaseType>;

        // Boolean
        if constexpr (std::is_same_v<NonRefType, bool>)
        {
            Type output = GetBoolean();
            PopStack(1);
            return output;
        }

        // CString
        else if constexpr (std::is_trivially_assignable_v<const char*&, Type>)
        {
            Type output = GetString();
            PopStack(1);
            return output;
        }

        // Integer
        else if constexpr (std::is_integral_v<NonRefType>)
        {
            Type output = GetInteger();
            PopStack(1);
            return output;  
        }

        // Float
        else if constexpr (std::is_floating_point_v<NonRefType>)
        {
            Type output = GetNumber();
            PopStack(1);
            return output;
        }

        // TODO: Functions, UserData, std::string
        else
        {
            LogError("LUA: Failed to get Lua element! Type was not a valid Lua (or currently supported) Type");
            return {};
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set an element in a lua table.
    ///		@tparam Type : Type of element we are trying to set.
    ///		@param tableName : Name of the table we are trying to access.
    ///		@param elementName : Name of the element we want to set.
    ///		@param val : Value we want to set.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename Type>
    void LuaSystem::SetElementInTable(const char* tableName, const char* elementName, Type&& val) const
    {
        using BaseType = std::remove_cv_t<Type>;
        using NonRefType = std::remove_reference_t<BaseType>;

        // Check if the table exists.
        if (!GetTable(tableName))
        {
            LogError("LUA: Failed to set element in Lua table named '%'!", tableName);
            return;
        }

#if _DEBUG
        // Check to see if the element exists for that table.
        if (!GetElement(elementName))
        {
            LogWarning("LUA: Failed to set element in Lua table named '%'! No element named '%' exists!", tableName, elementName);
            PopStack(2);
            return;
        }

        PopStack(1);

#endif

        // Push the name of the element onto the stack.
        PushString(elementName);

        // Push the value of the Element onto the stack.

        // Boolean
        if constexpr (std::is_same_v<NonRefType, bool>)
        {
            PushBool(val);
        }

        // CString
        else if constexpr (std::is_trivially_assignable_v<const char*&, Type>)
        {
            PushString(val);
        }

        // Integer
        else if constexpr (std::is_integral_v<NonRefType>)
        {
            PushInteger(val);
        }

        // Float
        else if constexpr (std::is_floating_point_v<NonRefType>)
        {
            PushFloat(val);
        }

        // TODO: Functions, UserData, std::string

        else
        {
            LogError("LUA: Failed to set Lua element! Type was not a valid Lua (or currently supported) Type");
            return;
        }

        // Set the Element.
        SetElement();
    }


    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Push an argument of any type onto the lua stack.
    ///		@tparam Type : Type of argument.
    ///		@param val : Value of the argument.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename Type>
    bool LuaSystem::Push(Type&& val) const
    {
        using BaseType = std::remove_cv_t<Type>;
        using NonRefType = std::remove_reference_t<BaseType>;

        // Boolean
        if constexpr (std::is_same_v<NonRefType, bool>)
        {
            PushBool(val);
        }

        // CString
        if constexpr (std::is_trivially_assignable_v<const char*&, Type>)
        {
            PushString(val);
        }

        // Integer
        else if constexpr (std::is_integral_v<NonRefType>)
        {
            PushInteger(static_cast<long long>(val));   
        }

        // Float
        else if constexpr (std::is_floating_point_v<NonRefType>)
        {
            PushFloat(static_cast<double>(val));
        }

        // TODO: Functions, UserData, std::strings

        else
        {
            LogError("LUA: Failed to push Lua parameter! Type was not a valid Lua (or currently supported) Type");
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Push any number of parameters for a function onto the stack. Recursively moves through the parameter pack.
    ///		@tparam Param : Type of the first parameter.
    ///		@tparam OtherParamTypes : The other parameter types that we will get to.
    ///		@param paramCount : Tracker for the number of parameters we have pushed.
    ///		@param param : The first parameter.
    ///		@param otherParams : The other parameters that we will unpack.
    ///		@returns : If there was an issue pushing the parameters, then it returns false. Otherwise true.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename Param, typename... OtherParamTypes>
    bool LuaSystem::PushParams(int& paramCount, Param&& param, OtherParamTypes&&... otherParams) const
    {
        if (!Push(std::forward<Param>(param)))
        {
            LogError("LUA: Failed to push lua parameter!");

            // Pop off each parameter that we have pushed.
            PopStack(paramCount);

            return false;
        }

        // Increase the param count.
        ++paramCount;

        // Recurse until all of the params are pushed.
        return PushParams(paramCount, otherParams...);
    }
}