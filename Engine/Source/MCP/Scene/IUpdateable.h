#pragma once
// IUpdateable.h
#include <cstdint>

namespace mcp
{
    using IUpdateableId = uint32_t;

    class IUpdateable
    {
        static inline IUpdateableId s_idCounter = 0;
    protected:
        const IUpdateableId m_updateableId;

    public:
        IUpdateable();
        virtual ~IUpdateable() = default;
        virtual void Update(const float deltaTimeMs) = 0;

        [[nodiscard]] IUpdateableId GetUpdateId() const { return m_updateableId; }
    };
}