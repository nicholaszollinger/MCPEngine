#pragma once
// LuaResource.h

#include <cstdint>

struct lua_State;

namespace mcp
{
    using LuaResourceId = int;

    class LuaResource
    {
        friend class LuaResourcePtr;
        static constexpr LuaResourceId kInvalidId = -1;

        lua_State* m_pState;
        uint32_t m_refCount;
        LuaResourceId m_id;

        LuaResource(lua_State* pState, const LuaResourceId id);
    public:
        ~LuaResource();
        void AddRef() { ++m_refCount; }
        bool RemoveRef();

        [[nodiscard]] uint32_t GetCount() const { return m_refCount; }
        [[nodiscard]] LuaResourceId GetId() const { return m_id; }
        [[nodiscard]] bool IsValid() const { return m_pState && m_id != kInvalidId; }
    };

    class LuaResourcePtr
    {
        friend class LuaSystem;
    private:
        LuaResource* m_pResource;

        // Private constructor: Only the Lua System can create the initial pointer.
        LuaResourcePtr(lua_State* pState, const LuaResourceId id);
    public:
        // Constructs an invalid ResourcePtr.
        LuaResourcePtr();
        LuaResourcePtr(const LuaResourcePtr& right);
        LuaResourcePtr(LuaResourcePtr&& right) noexcept;
        LuaResourcePtr& operator=(const LuaResourcePtr& right);
        LuaResourcePtr& operator=(LuaResourcePtr&& right) noexcept;
        ~LuaResourcePtr();

        [[nodiscard]] LuaResourceId GetId() const;
        [[nodiscard]] bool IsValid() const;
    };
}
