#pragma once
// IUpdateable.h
#include <cstdint>

namespace mcp
{
    using UpdateableId = uint32_t;

    class IUpdateable
    {
        static inline UpdateableId s_idCounter = 0;
    protected:
        const UpdateableId m_updateableId;

    public:
        IUpdateable();
        virtual ~IUpdateable() = default;
        virtual void Update(const float deltaTimeMs) = 0;

        [[nodiscard]] UpdateableId GetUpdateId() const { return m_updateableId; }
    };
}