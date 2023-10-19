#pragma once
// Font.h

#include <string>
#include "Resource.h"

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

    class Font final : public Resource<FontResourceRequest>
    {
    public:
        MCP_DEFINE_RESOURCE_DESTRUCTOR(Font)

        virtual void Free() override;
        void SetSize(const int size) const;

    private:
        virtual void* LoadResourceType() override;
    };
}