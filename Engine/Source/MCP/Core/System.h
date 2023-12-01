#pragma once
// System.h

#include "TypeFactory.h"

namespace mcp
{
    class System;
    using SystemFactory = TypeFactory<System>;

#define MCP_DEFINE_SYSTEM(typename)                                 \
private:                                                            \
    friend class Application;                                       \
    typename() = default;                                           \
    virtual ~typename() override = default;                         \
                                                                    \
public:                                                             \
    typename(const typename&) = delete;                             \
    typename(typename&&) noexcept = delete;                         \
    typename& operator=(const typename&) = delete;                  \
    typename& operator=(typename&&) noexcept = delete;              \
                                                                    \
    MCP_REGISTER_TYPE(SystemFactory, typename)                      \


#define MCP_DEFINE_STATIC_SYSTEM_GETTER(typename)                           \
typename* typename::Get()                                               \
{                                                                       \
    auto* pSystem = mcp::Application::Get()->GetSystem<typename>();     \
    MCP_CHECK(pSystem);                                                 \
    return pSystem;                                                     \
}
    

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : A System is a container for game-wide functionality. There will only ever be a single instance of a System, which
    ///         is created by the Application class during start up. All engine systems will be created before any game systems.
    ///         An example of an Engine-level System is the GraphicsSystem. An example of a GameSystem could be a 'SettingsSystem'.   
    //-----------------------------------------------------------------------------------------------------------------------------
    class System
    {
        friend class Application;
    protected:
        System() = default;
        virtual ~System() = default;

    public:
        // Copying/Moving/Assignment are not allowed.
        System(const System&) = delete;
        System(System&&) noexcept = delete;
        System& operator=(const System&) = delete;
        System& operator=(System&&) noexcept = delete;

        [[nodiscard]] virtual TypeId GetTypeId() const = 0;

    protected:
        virtual bool Init() = 0;
        virtual void Close() = 0;
    };
}