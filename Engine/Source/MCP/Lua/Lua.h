#pragma once
// Lua.h

#include "LuaSystem.h"

namespace mcp::lua
{
    // Load Lua Scripts
    bool LoadScript(const char* pFilepath);
    LuaResourcePtr LoadScriptInstance(const char* pScriptFilepath);
    LuaResourcePtr LoadScriptInstance(const char* pScriptFilepath, const char* pScriptDataPath);

    // Creating a Table
    LuaResourcePtr CreateTable();

    // Get Global Variables
    std::optional<bool> GetBoolean(const char* varName);
    std::optional<int64_t> GetInteger(const char* varName);
    std::optional<double> GetNumber(const char* varName);
    const char* GetString(const char* varName);

    // Accessing Tables
    template <typename ElementType> std::optional<ElementType> GetElementInTable(const char* tableName, const char* elementName);
    template<typename Type> void SetElementInTable(const char* tableName, const char* elementName, Type&& val);

    template<typename ElementType> void SetElementInTable(LuaResourcePtr&& tableResource, const char* elementName, ElementType&& val);

    // Get Global Helpers
    template<typename IntegralType> std::optional<IntegralType> GetIntegerAs(const char* varName);
    template<typename FloatingPointType> std::optional<FloatingPointType> GetNumberAs(const char* varName);

    // Set Global Variables
    void SetString(const char* varName, const char* value);
    void SetBoolean(const char* varName, const bool value);
    void SetInteger(const char* varName, const int64_t& value);
    void SetNumber(const char* varName, const double value);

    // Calling Functions
    template<typename...Args>
    void CallFunction(const char* pFunctionName, Args&&...args);

    template<typename...Args>
    void CallMemberFunction(const LuaResourcePtr resource, const char* pMemberFunctionName, Args&&...args);

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Internal class to hold onto the Engine-wide LuaSystem. You don't need to interact with this at, just call
    ///         the functions provided.
    //-----------------------------------------------------------------------------------------------------------------------------
    class LuaLayer final : public IProcess
    {
        DEFINE_GLOBAL_MANAGER(LuaLayer)
        LuaSystem m_system;

    public:
        [[nodiscard]] LuaSystem& GetSystem() { return m_system; }

    private:
        virtual bool Init() override;
        virtual void Close() override;
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    // TEMPLATE FUNCTION DEFINITIONS
    //-----------------------------------------------------------------------------------------------------------------------------

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get an element in a table.
    ///		@tparam ElementType : Type of the element you want to get.
    ///		@param tableName : Name of the table you are accessing.
    ///		@param elementName : Name of the element in the table.
    ///		@returns : std::optional<ElementType> that either has a value or not.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename ElementType>
    std::optional<ElementType> GetElementInTable(const char* tableName, const char* elementName)
    {
        return LuaLayer::Get()->GetSystem().GetElementInTable<ElementType>(tableName, elementName);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set an element in a table.
    ///		@tparam Type : Type of value we are going to set an element to.
    ///		@param tableName : Name of the table you are accessing.
    ///		@param elementName : Name of the element you want to set.
    ///		@param val : Value you want to set it to.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename Type>
    void SetElementInTable(const char* tableName, const char* elementName, Type&& val)
    {
        LuaLayer::Get()->GetSystem().SetElementInTable<Type>(tableName, elementName, std::forward<Type>(val));
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set an element in a table resource.
    ///		@tparam ElementType : Type of value we are going to set an element to.
    ///		@param tableResource : Resource that we are editing. 
    ///		@param elementName : Name of the element you want to set.
    ///		@param val : Value you want to set it to.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename ElementType>
    void SetElementInTable(const LuaResourcePtr& tableResource, const char* elementName, ElementType&& val)
    {
        LuaLayer::Get()->GetSystem().SetElementInTable<ElementType>(tableResource, elementName, std::forward<ElementType>(val));
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get a global Integer from among loaded lua scripts, cast to the integral type of your choice.
    ///		@tparam IntegralType : Integral type you want to cast the int64_t value to.
    ///		@param varName : Name of the global variable you want to get.
    ///		@returns : The value of the integer. NOTE: If this function fails, this will just return 0!
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename IntegralType>
    std::optional<IntegralType> GetIntegerAs(const char* varName)
    {
        static_assert(std::is_integral_v<IntegralType>, "ElementType must be an integral type!");

        if (const auto value = GetInteger(varName)) 
            return std::optional<IntegralType>(static_cast<IntegralType>(*value));
        return {};
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Get a global Integer from among loaded lua scripts, cast to the floating-point type of your choice.
    ///		@tparam FloatingPointType : Floating-point type you want to cast the double value to.
    ///		@param varName : Name of the global variable you want to get.
    ///		@returns : The value of the global double. NOTE: If this function fails, this will just return 0.0!
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename FloatingPointType>
    std::optional<FloatingPointType> GetNumberAs(const char* varName)
    {
        static_assert(std::is_floating_point_v<FloatingPointType>, "ElementType must be an floating-point type!");

        if (const auto value = GetInteger(varName))
            return std::optional<FloatingPointType>(static_cast<FloatingPointType>(*value));
        return {};
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Call a lua function from among loaded lua scripts.
    ///		@tparam Args : Parameter types required for the function.
    ///		@param pFunctionName : Name of the function you want to call.
    ///		@param args : Parameters you want to pass to the function.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename...Args>
    void CallFunction(const char* pFunctionName, Args&&...args)
    {
        LuaLayer::Get()->GetSystem().CallFunction(pFunctionName, args...);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Call a member function of a loaded lua Object.
    ///		@tparam Args : Parameter types required for the function.
    ///		@param resource : Pointer to the resource that we are using.
    ///		@param pMemberFunctionName : Name of the member function
    ///		@param args : Parameter value passed into the function.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename... Args>
    void CallMemberFunction(const LuaResourcePtr resource, const char* pMemberFunctionName, Args&&... args)
    {
        LuaLayer::Get()->GetSystem().CallMemberFunction(resource, pMemberFunctionName, args...);
    }
}