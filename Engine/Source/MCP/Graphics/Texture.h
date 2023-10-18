#pragma once
// Texture.h

#include "MCP/Core/Resource/Resource.h"
#include "Utility/Types/Color.h"
#include "Utility/Types/Vector2.h"

// TODO: Need to figure out what I want to do as far as copying, copy-assignment, etc.

namespace mcp
{
    struct TextureData
    {
        TextureData(void* pTexture, const int width, const int height);

        void* pTexture = nullptr;   // Pointer to the actual texture resource.
        int width = 0;              // Base image width
        int height = 0;             // Base image height
    };

    class Texture final : public Resource
    {
    public:
        Texture() = default;
        virtual ~Texture() override;

        virtual void Load(const char* pFilePath, const char* pPackageName = nullptr, const bool isPersistent = false) override;

        [[nodiscard]] virtual void* Get() const override;
        [[nodiscard]] Vec2Int GetTextureSize() const;
        [[nodiscard]] Vec2 GetTextureSizeAsVec2() const;

        
    protected:
        virtual void Free() override;
    };
}
