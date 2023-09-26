#pragma once
// Font.h

#include "Resource.h"
#include "Utility/Types/EnumHelpers.h"

namespace mcp
{
   /* enum class FontStyle
    {
        kNone,
        kBold,
        kItalic,
        kUnderline,
        kStrikeThrough,
    };

    DEFINE_ENUM_CLASS_BITWISE_OPERATORS(FontStyle)*/

    class Font final : public Resource
    {
        int m_fontSize = 0;

    public:
        virtual ~Font() override;

        void Load(const char* pFilepath, const int fontSize, const char* pPackageName = nullptr, const bool isPersistent = false);
        virtual void Free() override;

        void SetSize(const int size) const;
        //void SetStyle(const FontStyle style);

    private:
        virtual void Load(const char* pFilePath, const char* pPackageName, const bool isPersistent) override;
    };
}