#pragma once
// Texture.h

#include "MCP/Core/Resource/Resource.h"
#include "utility/Vector2.h"

// TODO: Need to figure out what I want to do as far as copying, copy-assignment, etc.

namespace mcp
{
    class Texture final : public Resource
    {
        Vec2Int m_baseSize;

    public:
        Texture() = default;
        virtual ~Texture() override;

        virtual void Load(const char* pFilePath, const char* pPackageName = nullptr, const bool isPersistent = false) override;
        [[nodiscard]] const Vec2Int& GetBaseSize() const { return m_baseSize; }
        [[nodiscard]] Vec2 GetBaseSizeAsFloat() const { return { static_cast<float>(m_baseSize.x), static_cast<float>(m_baseSize.y) }; }
        
    protected:
        virtual void Free() override;
    };
}
