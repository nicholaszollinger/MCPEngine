#pragma once
// Localization.h

#include "MCP/Core/System.h"
#include "MCP/Core/Event/MulticastDelegate.h"

namespace mcp
{
    class LanguageSetEvent;
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : System for storing and managing localized text for each supported language.
    //-----------------------------------------------------------------------------------------------------------------------------
    class LocalizationSystem final : public System
    {
    public:
        using Token = StringId;

    private:
        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : A container of each localized string of the loaded tokens.
        //-----------------------------------------------------------------------------------------------------------------------------
        using LocalizedStringContainer = std::vector<std::string>;
        using TokenIdMap = std::unordered_map<Token, size_t, StringIdHasher>;

        struct LocalizedData
        {
            // A vector of vectors, each element in the outside vector is a language, and each inside vector is
            // the localized string version of the loaded tokens. This is like a SoA.
            std::vector<LocalizedStringContainer> localizedTexts;

            // Maps a token to an index in the LocalizedStringContainers.
            TokenIdMap tokenToIndexMap;

            // Vector of supported languages.
            std::vector<StringId> languages;
        };

        MCP_DEFINE_SYSTEM(LocalizationSystem)

        LocalizedData m_data;
        size_t m_currentLanguage;

    public:
        LocalizationSystem(const size_t defaultLanguage, LocalizedData&& data);

        void LoadLocalizedScript(const char* pFilepath);
        void SetLanguage(StringId language);
        [[nodiscard]] const std::string* GetLocalized(const Token token);

        static LocalizationSystem* Get();
        static LocalizationSystem* AddFromData(const XMLElement element);

    private:
        virtual bool Init() override;
        virtual void Close() override;

        static void LoadScript(const char* filepath, LocalizedData& data);
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : A Localized string can contain localized tokens within the string. When the language of the Application changes, 
    ///         this string will update its value. This stores both the unformatted and formatted version of the string.
    //-----------------------------------------------------------------------------------------------------------------------------
    class LocalizedString
    {
    public:
        using OnStringUpdated = MulticastDelegate<>;
        
        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //		
        ///		@brief : Called when the formatted string was updated due to the Application's language being changed.
        ///         \n NOTE: This is not called after Set() or when appending the string. Those changes are assumed
        ///         to be known by the owner of the LocalizedString.
        //-----------------------------------------------------------------------------------------------------------------------------
        OnStringUpdated m_onStringUpdated;

    private:
        std::string m_unformatted;  // The unformatted string. Example: "{Player_name} is a cool person".
        std::string m_formatted;    // The formatted string, with any tokens replaced with their values.

    public:
        LocalizedString() = default;
        LocalizedString(std::string string);
        LocalizedString(LocalizedString&& right) noexcept;
        LocalizedString(const LocalizedString& right);
        ~LocalizedString();

        LocalizedString& operator=(const LocalizedString& right);
        LocalizedString& operator=(const char* right);
        LocalizedString& operator=(const std::string& string);
        LocalizedString& operator=(LocalizedString&& right) noexcept;
        LocalizedString& operator+=(const char character);
        [[nodiscard]] char operator[](const std::string::size_type pos) const;

        void Set(const char* value);
        void Set(const std::string& value);
        void pop_back();

        [[nodiscard]] const std::string& GetUnformatted() const { return m_unformatted; }
        [[nodiscard]] const std::string& Get() const { return m_formatted; }
        [[nodiscard]] bool empty() const { return m_formatted.empty(); }
        [[nodiscard]] size_t size() const { return m_formatted.size(); }

    private:
        void OnLanguageSet(const LanguageSetEvent& event);
        void UpdateFormattedString();
    };

}
