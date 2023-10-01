#pragma once
// Entity.h

#include <cstdint>

namespace mcp
{
    class Scene;

    using EntityId = uint64_t;

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : An entity is the base class object that can be in a Scene.
    //-----------------------------------------------------------------------------------------------------------------------------
    class Entity
    {
        static inline EntityId s_idCounter = 0;

        Scene* m_pScene = nullptr;
        EntityId m_id;
        bool m_isActive = true;
        bool m_isQueuedForDeletion = false;

    public:
        Entity(Scene* pScene)
            : m_pScene(pScene)
            , m_id(s_idCounter++)
        {
            //
        }

        virtual ~Entity() = default;

        virtual void Destroy() = 0;
        virtual void SetActive(const bool isActive) { m_isActive = isActive; }
        [[nodiscard]] bool IsActive() const { return m_isActive; }
        [[nodiscard]] bool IsQueuedForDeletion() const { return m_isQueuedForDeletion; }
    };

    
}