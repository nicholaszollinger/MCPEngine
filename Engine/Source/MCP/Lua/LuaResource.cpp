// LuaResource.cpp

#include "LuaResource.h"

#include "BleachNew.h"
#include "LuaSource.h"
#include "MCP/Debug/Assert.h"

namespace mcp
{
    LuaResource::LuaResource(lua_State* pState, const LuaResourceId id)
        : m_pState(pState)
        , m_refCount(1)
        , m_id(id)
    {
        //
    }

    LuaResource::~LuaResource()
    {
        MCP_CHECK(m_pState);
        MCP_CHECK_MSG(m_refCount == 0, "Tried to free a LuaResource that still had references to it!");

        // Free the lua resource from the registry.
        luaL_unref(m_pState, LUA_REGISTRYINDEX, m_id);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Remove a reference to this Resource. If there are no more references to the resource, then this returns true;
    ///         it is time to clean up the resource. If we still have resources pointing to it, it returns false.
    //-----------------------------------------------------------------------------------------------------------------------------
    bool LuaResource::RemoveRef()
    {
        --m_refCount;
        if (m_refCount == 0)
            return true;

        return false;
    }

    LuaResourcePtr::LuaResourcePtr()
        : m_pResource(nullptr)
    {
        //
    }

    LuaResourcePtr::LuaResourcePtr(lua_State* pState, const LuaResourceId id)
        : m_pResource(nullptr)
    {
        // If we are creating a valid resource, new it up.
        if (id != LuaResource::kInvalidId)
        {
            m_pResource = BLEACH_NEW(LuaResource(pState, id));
        }
    }

    LuaResourcePtr::LuaResourcePtr(const LuaResourcePtr& right)
    {
        // Copy the resource ptr, and add a reference if it is valid.
        m_pResource = right.m_pResource;
        if (m_pResource)
            m_pResource->AddRef();
    }

    LuaResourcePtr::LuaResourcePtr(LuaResourcePtr&& right) noexcept
    {
        // Take ownership of the resource.
        m_pResource = right.m_pResource;
        right.m_pResource = nullptr;
    }

    LuaResourcePtr& LuaResourcePtr::operator=(const LuaResourcePtr& right)
    {
        // If this is not self referential and we have different resources,
        if (this != &right && m_pResource != right.m_pResource)
        {
            // If we already held a resource,
            if (m_pResource)
            {
                // Remove a reference to our old resource.
                m_pResource->RemoveRef();
            }

            // Copy the resource and add a reference if it is valid.
            m_pResource = right.m_pResource;
            if (m_pResource)
                m_pResource->AddRef();
        }

        return *this;
    }

    LuaResourcePtr& LuaResourcePtr::operator=(LuaResourcePtr&& right) noexcept
    {
        // If this is not self referential and we have different resources,
        if (this != &right && m_pResource != right.m_pResource)
        {
            // If we already held a resource,
            if (m_pResource)
            {
                // Remove a reference to our old resource.
                m_pResource->RemoveRef();
            }

            // Take ownership of the resource.
            m_pResource = right.m_pResource;
            right.m_pResource = nullptr;
        }

        return *this;
    }

    LuaResourcePtr::~LuaResourcePtr()
    {
        if (m_pResource && m_pResource->RemoveRef())
        {
            //MCP_LOG("Lua", "Freeing LuaResource... Id: ", m_pResource->GetId());
            BLEACH_DELETE(m_pResource);
            m_pResource = nullptr;
        }
    }

    LuaResourceId LuaResourcePtr::GetId() const
    {
        if (m_pResource)
        {
            return m_pResource->GetId();
        }

        return LuaResource::kInvalidId;
    }

    bool LuaResourcePtr::IsValid() const
    {
        return m_pResource && m_pResource->GetId() != LuaResource::kInvalidId;
    }
}
