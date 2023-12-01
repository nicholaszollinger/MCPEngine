// Localization.cpp

#include "Localization.h"
#include "MCP/Core/Application/Application.h"
#include "MCP/Core/Event/ApplicationEvent.h"

namespace mcp
{
    class LanguageSetEvent final : public ApplicationEvent
    {
        MCP_DEFINE_EVENT_ID(LanguageSetEvent)

    public:
        LanguageSetEvent() = default;
    };

    MCP_DEFINE_STATIC_SYSTEM_GETTER(LocalizationSystem)

    LocalizationSystem::LocalizationSystem(const size_t defaultLanguage, LocalizedData&& data)
        : m_data(std::move(data))
        , m_currentLanguage(defaultLanguage)
    {
        //
    }

    bool LocalizationSystem::Init()
    {
        return true;   
    }

    void LocalizationSystem::Close()
    {
        
    }

    void LocalizationSystem::LoadLocalizedScript(const char* pFilepath)
    {
        LoadScript(pFilepath, m_data);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      TODO: Should this send out an event to all text?
    //		
    ///		@brief : Set the language for all localized text.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LocalizationSystem::SetLanguage(StringId language)
    {
        for (size_t i = 0; i < m_data.languages.size(); ++i)
        {
            if (language == m_data.languages[i])
            {
                m_currentLanguage = i;

                // Send a global event to all that depend on the language:
                LanguageSetEvent event;
                GlobalEventDispatcher::InvokeNow<LanguageSetEvent>(event);

                return;
            }
        }

        MCP_WARN("LocalizationSystem", "Failed to set language! ", *language, " is not a supported language!");
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the localized version of the token for the currently set language. If the token isn't valid, it will
    ///         just return the token as a string.
    //-----------------------------------------------------------------------------------------------------------------------------
    const std::string* LocalizationSystem::GetLocalized(const Token token)
    {
        const auto result = m_data.tokenToIndexMap.find(token);

        // If we don't have the token, return the value of the token:
        if ( result == m_data.tokenToIndexMap.end())
        {
            MCP_WARN("LocalizationSystem", "Failed to find localized text for token '", *token , "'. Returning token value...");
            return token.GetConstPtr();
        }

        const size_t index = result->second;
        const auto& stringContainer = m_data.localizedTexts[m_currentLanguage];
        
        return &stringContainer[index];
    }


    LocalizationSystem* LocalizationSystem::AddFromData(const XMLElement element)
    {
        const auto localizationElement = element.GetChildElement("Localization");
        if (!localizationElement.IsValid())
        {
            MCP_ERROR("LocalizationSystem", "Failed to load from data! No Localization element found!");
            return nullptr;
        }

        const auto languagesElement = localizationElement.GetChildElement("SupportedLanguages");
        if (!languagesElement.IsValid())
        {
            MCP_ERROR("LocalizationSystem", "Failed to load from data! No Localization element found!");
            return nullptr;
        }

        LocalizedData data;


        // Get the supported languages:
        auto language = languagesElement.GetChildElement("Language");
        while (language.IsValid())
        {
            StringId languageName = language.GetAttributeValue<const char*>("token", nullptr);
            if (!languageName.IsValid())
            {
                MCP_ERROR("LocalizationSystem", "Failed to add supported language! Language Element's 'token' was nullptr!");
                language = language.GetSiblingElement("Language");
                continue;
            }

            // Add the Language
            data.languages.emplace_back(languageName);

            // Add a new StringContainer for the language
            data.localizedTexts.emplace_back();

            language = language.GetSiblingElement("Language");
        }

        // Get the default language index:
        const StringId defaultLanguage = languagesElement.GetAttributeValue<const char*>("default");
        size_t defaultLanguageIndex = 0;

        if (defaultLanguage.IsValid())
        {
            for (size_t i = 0; i < data.languages.size(); ++i)
            {
                if (defaultLanguage == data.languages[i])
                {
                    defaultLanguageIndex = i;
                    break;
                }
            }
        }

        // Load any localized scripts:
        const auto scriptsElement = localizationElement.GetChildElement("Scripts");
        auto file = scriptsElement.GetChildElement("File");

        while (file.IsValid())
        {
            const auto* path = file.GetAttributeValue<const char*>("path");
            if (!path)
            {
                MCP_ERROR("LocalizationSystem", "Failed to find a path for LocalizationScript!");
                file = file.GetSiblingElement("File");
                continue;
            }

            // Load the script and add it to our data
            LoadScript(path, data);

            file = file.GetSiblingElement("File");
        }

        return BLEACH_NEW(LocalizationSystem(defaultLanguageIndex, std::move(data)));
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Load a script from an XMLFile and add its data to the LocalizedData.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LocalizationSystem::LoadScript(const char* filepath, LocalizedData& data)
    {
        if (data.languages.empty())
        {
            MCP_WARN("LocalizationSystem", "Failed to load localized script! There are no supported langauges!");
            return;
        }

        XMLParser parser;
        if (!parser.LoadFile(filepath))
        {
            MCP_ERROR("LocalizationSystem", "Failed to load localized script! Failed to parse file at path: ", filepath);
            return;
        }

        const auto localizedText = parser.GetElement("LocalizedText");
        if (!localizedText.IsValid())
        {
            MCP_ERROR("LocalizationSystem", "Failed to load localized script! Failed to find LocalizedText Element!");
            return;
        }

        auto text = localizedText.GetChildElement("Text");

        while (text.IsValid())
        {
            StringId token = text.GetAttributeValue<const char*>("token", nullptr);
            if (!token.IsValid())
            {
                MCP_ERROR("LocalizationSystem", "Failed to load Localized text! Text had no token value!");
                text = text.GetSiblingElement("Text");
                continue;
            }

            // Add the Token to index mapping:
            data.tokenToIndexMap.emplace(token, data.localizedTexts[0].size());

            // Add an empty string for each container.
            for (auto& stringContainer : data.localizedTexts)
            {
                stringContainer.emplace_back();
            }

            // Add each translation for this token:
            auto translation = text.GetChildElement();

            while (translation.IsValid())
            {
                StringId languageValue = translation.GetName();

                // O(N) every time, but the array is probably small and the comparison is quick. *Shrug* for now.
                for (size_t i = 0; i < data.languages.size(); ++i)
                {
                    if (languageValue == data.languages[i])
                    {
                        data.localizedTexts[i].back() = translation.GetText(); 
                        break;
                    }
                }

                translation = translation.GetSiblingElement();
            }

            text = text.GetSiblingElement("Text");
        }
    }

    LocalizedString::LocalizedString(std::string string)
        : m_unformatted(std::move(string))
    {
        // Register for LanguageSetEvents
        MCP_ADD_MEMBER_FUNC_EVENT_LISTENER(LanguageSetEvent, OnLanguageSet);

        UpdateFormattedString();
    }

    LocalizedString::~LocalizedString()
    {
        // Unregister from LocalizationEvents
        GlobalEventDispatcher::RemoveListener<LanguageSetEvent>(this);
    }

    LocalizedString::LocalizedString(const LocalizedString& right)
        : m_unformatted(right.m_unformatted)
        , m_formatted( right.m_formatted)
    {
        // Register for LanguageSetEvents
        MCP_ADD_MEMBER_FUNC_EVENT_LISTENER(LanguageSetEvent, OnLanguageSet);
    }

    LocalizedString::LocalizedString(LocalizedString&& right) noexcept
        : m_unformatted(std::move(right.m_unformatted))
        , m_formatted(std::move(right.m_formatted))
    {
        // Register for LanguageSetEvents
        MCP_ADD_MEMBER_FUNC_EVENT_LISTENER(LanguageSetEvent, OnLanguageSet);
    }

    LocalizedString& LocalizedString::operator=(const LocalizedString& right)
    {
        if (this != & right)
        {
            m_unformatted = right.m_unformatted;
            m_formatted = right.m_formatted;
        }

        return *this;
    }

    LocalizedString& LocalizedString::operator=(const char* right)
    {
        m_unformatted = right;
        UpdateFormattedString();

        return *this;
    }

    LocalizedString& LocalizedString::operator=(const std::string& string)
    {
        return *this = string.c_str();
    }

    LocalizedString& LocalizedString::operator=(LocalizedString&& right) noexcept
    {
        if (this != & right)
        {
            m_unformatted = std::move(right.m_unformatted);
            m_formatted = std::move(right.m_formatted);
        }

        return *this;
    }

    LocalizedString& LocalizedString::operator+=(const char character)
    {
        m_unformatted += character;
        m_formatted += character;
        return *this;
    }

    char LocalizedString::operator[](const std::string::size_type pos) const
    {
        return m_formatted[pos];
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the unformatted version of the String.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LocalizedString::Set(const char* value)
    {
        m_unformatted = value;
        UpdateFormattedString();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Set the unformatted version of the String.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LocalizedString::Set(const std::string& value)
    {
        m_unformatted = value;
        UpdateFormattedString();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      WARNING: This can mess up the unformatted string!!! What if we pop_back part of the token?
    //		
    ///		@brief : Same as std::string::pop_back(), just applies the pop to both the unformatted and the formatted string.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LocalizedString::pop_back()
    {
        m_unformatted.pop_back();
        m_formatted.pop_back();
    }

    void LocalizedString::OnLanguageSet([[maybe_unused]] const LanguageSetEvent& event)
    {
        UpdateFormattedString();
        m_onStringUpdated.Broadcast();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      With long strings, this can become a huge bottleneck for performance. This is something that may need to be threaded in
    //      the future.
    //		
    ///		@brief : Updates our formatted string.
    //-----------------------------------------------------------------------------------------------------------------------------
    void LocalizedString::UpdateFormattedString()
    {
        m_formatted = m_unformatted;

        if (m_formatted.empty())
            return;

        auto* pSystem = LocalizationSystem::Get();

        size_t start = m_formatted.find("%L{", 0);
        while (start != std::string::npos)
        {
            size_t end = m_formatted.find("}%", start);

            const StringId token = m_formatted.substr(start + 3, end - (start + 3));
            auto* replacedValue = pSystem->GetLocalized(token);
            m_formatted = m_formatted.replace(start, end - (start) + 2, *replacedValue);

            end = start + replacedValue->size();
            start = m_formatted.find("%L{", end);
        }
    }

}