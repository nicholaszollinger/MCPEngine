#pragma once
// TextFieldWidget.h

#include "ButtonWidget.h"
#include "TextWidget.h"
#include "MCP/Core/Event/MulticastDelegate.h"
#include "MCP/Scene/IUpdateable.h"

namespace mcp
{
    class ImageWidget;
    class Font;

    struct TextFieldData
    {
        TextFormatData format;
        const char* backgroundImagePath = nullptr;
        const char* cursorImagePath = nullptr;
        const char* defaultText = nullptr;
        int maxCharCount = -1;
        float cursorBlinkSpeed;
    };

    class TextFieldWidget final : public ButtonWidget, public IUpdateable
    {
    public:
        using OnValueChangedDelegate = MulticastDelegate<const std::string&>;

        // Called after the input string has changed NOTE: Not during each entry of a letter, but when the user is finished editing. 
        OnValueChangedDelegate m_onValueChanged;

    private:
        MCP_DEFINE_WIDGET(TextFieldWidget)
            
        std::string m_defaultText;                  // The text displayed when nothing is entered in the text field.
        TextWidget* m_pTextWidget = nullptr;
        TextWidget* m_pCursor = nullptr;
        Widget* m_pPreviousFocused = nullptr;
        size_t m_maxCharCount = 0;                    // Maximum number of characters allowed in the Field. 0 Means unlimited.
        float m_cursorBlinkInterval = 1.f;
        float m_timeLeftToBlink = 1.f;
        bool m_valueEmpty;

    public:
        TextFieldWidget(const WidgetConstructionData& data, ButtonBehavior&& behavior, const TextFieldData& fieldData);

        virtual bool Init() override;
        virtual void Update(const float deltaTimeMs) override;
        [[nodiscard]] const std::string& GetText() const;

        static TextFieldWidget* AddFromData(const XMLElement element);

        //static void RegisterLuaFunctions(lua_State* pState);
    private:
        virtual void OnExecute(const Vec2 relativeClickPosition) override;
        virtual void OnHoverEnter() override;
        virtual void OnHoverExit() override;
        virtual void HandleKeyPress(KeyEvent& event) override;
        void CreateChildWidgets(const TextFieldData& data);
        void FinishText();
        void StopEditing();
    };
}
