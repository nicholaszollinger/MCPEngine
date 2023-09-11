#pragma once
// GlobalManager.h

#include <cassert>
#include <type_traits>
#include "BleachNew.h"
#include "Process.h"

namespace mcp
{
    namespace GlobalManagerInternal
    {
        template<typename ManagerType, typename = int>
        struct ManagerHasInstance : std::false_type {};

        template<typename ManagerType>
        struct ManagerHasInstance<ManagerType, decltype((void)ManagerType::s_pInstance, 0)> : std::true_type {};
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Defines the necessary interface for using a singleton GlobalManager.
    ///		@param GlobalManagerTypename : Name of the ManagerType.
    //-----------------------------------------------------------------------------------------------------------------------------
#define DEFINE_GLOBAL_MANAGER(GlobalManagerTypename)                                        \
    private:                                                                                \
        friend class GlobalManagerFactory;                                                  \
        friend class Application;                                                           \
        static inline GlobalManagerTypename* s_pInstance = nullptr;                         \
    public:                                                                                 \
        static GlobalManagerTypename* Get() { assert(s_pInstance); return s_pInstance; }    \
    private:

    class GlobalManagerFactory
    {
        friend class Application;

        template<typename ManagerType>
        static void Create()
        {
            static_assert(GlobalManagerInternal::ManagerHasInstance<ManagerType>::value, "ManagerType has no accessible member s_pInstance!");
            static_assert(std::is_convertible_v<ManagerType*, IProcess*>, "ManagerType* must be convertible to IProcess*!");

            if (!ManagerType::s_pInstance)
            {
                ManagerType::s_pInstance = BLEACH_NEW(ManagerType);
            }
        }

        template<typename ManagerType>
        static void Destroy()
        {
            static_assert(GlobalManagerInternal::ManagerHasInstance<ManagerType>::value, "ManagerType has no accessible member s_pInstance!");
            static_assert(std::is_convertible_v<ManagerType*, IProcess*>, "ManagerType* must be convertible to IProcess*!");

            BLEACH_DELETE(ManagerType::s_pInstance);
            ManagerType::s_pInstance = nullptr;
        }
    };
}