#pragma once
// TextFile.h

#include "Resource.h"
// This is a test for the compress and expand functions.

namespace mcp
{
    class TextFile final : public Resource
    {
    public:
        TextFile() = default;
        virtual ~TextFile() override;
        virtual void Load(const char* pFilePath, const char* pPackageName, const bool isPersistent) override;

        void Dump() const;

    protected:
        virtual void Free() override;
    };
}
