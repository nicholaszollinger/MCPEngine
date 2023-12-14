#pragma once
// Script.h

#include "MCP/Lua/Lua.h"
#include "MCP/Lua/LuaResource.h"

namespace mcp
{
    class Script
    {
        LuaResourcePtr m_resource;

    public:
        Script() = default;
        Script(LuaResourcePtr&& resource);
        Script(const LuaResourcePtr& resource);

        bool Load(const char* path, const char* dataPath);

        template<typename...Args>
        void Run(const char* pFuncName, Args&&...args);

        [[nodiscard]] bool IsValid() const { return m_resource.IsValid(); }
    };

    template <typename...Args>
    void Script::Run([[maybe_unused]] const char* pFuncName, [[maybe_unused]] Args&&... args)
    {
#if MCP_EDITOR
        // In the future, I could have a flag for 'run in editor' or something.
        return;

#else
        if (!IsValid())
            return;

        lua::CallMemberFunction(m_resource, pFuncName, args...);
#endif
    }
}