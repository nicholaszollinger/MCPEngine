// Script.cpp

#include "Script.h"

namespace mcp
{
    Script::Script(const LuaResourcePtr& resource)
        : m_resource(resource)
    {
        //        
    }

    Script::Script(LuaResourcePtr&& resource)
        : m_resource(std::move(resource))
    {
        //
    }

    bool Script::Load(const char* path, const char* dataPath)
    {
        m_resource = lua::LoadScriptInstance(path, dataPath);
        return m_resource.IsValid();
    }
}