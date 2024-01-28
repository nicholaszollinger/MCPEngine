#pragma once
// Font.h

#include <string>
#include "Resource.h"
#include "MCP/Graphics/Texture.h"

namespace mcp
{
    struct FontResourceRequest final : public DiskResourceRequest
    {
        int fontSize = 0;

        uint64_t operator()(const FontResourceRequest& request) const
        {
            return HashString64(request.path.GetCStr()) ^ std::hash<int>{}(request.fontSize);
        }

        bool operator==(const FontResourceRequest& right) const
        {
            return path == right.path && fontSize == right.fontSize;
        }
    };

    struct FontData
    {
        static constexpr size_t kGlyphCount = 128;

        std::vector<TextureData*> m_glyphTextures;
        void* pFontResource = nullptr;
    };

    struct GlyphData
    {
        TextureData* pTextureData = nullptr;    // Rendering information for the glyph
        int cursorAdvanceDist = 0;              // The distance we need to move the cursor.
    };

    class Font final : public Resource<FontResourceRequest>
    {
    public:
        MCP_DEFINE_RESOURCE_DESTRUCTOR(Font)
        virtual void Free() override;

        [[nodiscard]] virtual void* Get() const override;
        [[nodiscard]] const TextureData* GetGlyphTexture(const uint32_t glyph) const;
        [[nodiscard]] int GetNewlineDistance() const;
        [[nodiscard]] int GetFontHeight() const;
        [[nodiscard]] int GetNextCharDistance(const uint32_t lastGlyph, const uint32_t nextGlyph) const;
        [[nodiscard]] int GetCursorDistanceAfterGlyph(const uint32_t glyph) const;

    private:
        virtual void* LoadResourceType() override;
    };
}
