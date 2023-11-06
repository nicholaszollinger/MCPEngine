// TextFieldWidget.cpp

#include "TextFieldWidget.h"

#include "ImageWidget.h"
#include "LuaSource.h"
#include "MCP/Core/Application/Application.h"
#include "MCP/Core/Event/ApplicationEvent.h"
#include "MCP/Graphics/Graphics.h"
#include "MCP/Lua/Lua.h"
#include "MCP/Scene/UILayer.h"

namespace mcp
{
    TextFieldWidget::TextFieldWidget(const WidgetConstructionData& data, ButtonBehavior&& behavior, const TextFieldData& fieldData)
        : ButtonWidget(data, std::move(behavior))
        , m_defaultText{}
        , m_maxCharCount(fieldData.maxCharCount)
        , m_cursorBlinkInterval(fieldData.cursorBlinkSpeed)
        , m_constraints(fieldData.constraints)
        , m_valueEmpty(true)
    {
        if (fieldData.defaultText)
            m_defaultText = fieldData.defaultText;

        CreateChildWidgets(fieldData);
    }

    bool TextFieldWidget::Init()
    {
        //MCP_LOG("TextFieldWidget", "Rect: ", GetRect().ToString());

        if (!ButtonWidget::Init())
            return false;

        return true;
    }

    void TextFieldWidget::Update(const float deltaTimeMs)
    {
        m_timeLeftToBlink -= (deltaTimeMs / 1000.f);
        if (m_timeLeftToBlink < 0.f)
        {
            m_pCursor->ToggleActive();
            m_timeLeftToBlink = m_cursorBlinkInterval;
        }
    }

    void TextFieldWidget::OnExecute([[maybe_unused]] const Vec2 relativeClickPosition)
    {
        // If we are already focused, return.
        if (IsFocused())
            return;
        
        m_pPreviousFocused = m_pUILayer->GetFocused();

        // Set the focus of this Widget. We are the only Widget that should be active now.
        Focus();

        // If our value is empty, remove the default text.
        if (m_valueEmpty)
            m_pTextWidget->SetText("");

        // Set our cursor active.
        m_pCursor->SetActive(true);
        const auto cursorPos = m_pTextWidget->GetCursorPosAtEnd();
        m_pCursor->SetLocalPosition(cursorPos.x, cursorPos.y);

        // If our cursor blinks: Add us to the updateables and set the cursor active.
        if (m_cursorBlinkInterval > 0.f)
        {
            m_timeLeftToBlink = m_cursorBlinkInterval;
            GetLayer()->AddUpdateable(this);
        }

        // Call into the script implementation with the updated value.
        if (m_pOnExecuteScript.IsValid())
            lua::CallMemberFunction(m_pOnExecuteScript, "OnExecute");
    }

    void TextFieldWidget::OnHoverEnter()
    {
        if (!IsFocused())
            ButtonWidget::OnHoverEnter();
    }

    void TextFieldWidget::OnHoverExit()
    {
        if (!IsFocused())
            ButtonWidget::OnHoverExit();
    }

    void TextFieldWidget::HandleKeyPress(KeyEvent& event)
    {
        if (event.State() == ButtonState::kPressed)
        {
            switch (event.Key())
            {
                // Cancels the text entry:
                case MCPKey::Escape:
                {
                    m_pTextWidget->SetText(m_defaultText);
                    StopEditing();
                    break;
                }

                // Submits the entry.
                case MCPKey::Enter:
                {
                    FinishText();
                    break;
                }

                case MCPKey::Backspace:
                {
                    m_pTextWidget->PopBack();

                    // Move the Cursor
                    m_timeLeftToBlink = m_cursorBlinkInterval;
                    m_pCursor->SetActive(true);

                    const auto cursorPos = m_pTextWidget->GetCursorPosAtEnd();
                    m_pCursor->SetLocalPosition(cursorPos.x, cursorPos.y);

                    break;
                }

                // Don't allow spaces. TODO: this should be configurable.
                case MCPKey::Space: break;

                default: 
                {
                    // If we have hit a max count, then don't append anything.
                    if (m_maxCharCount > 0 && m_pTextWidget->GetTextLength() >= m_maxCharCount)
                        return;

                    if (!CanAppendCharacter(event.Key()))
                    {
                        return;
                    }

                    const auto glyphValue = static_cast<char>(event.GetGlyphValue());
                    m_pTextWidget->Append(glyphValue);

                    // Move the Cursor
                    m_timeLeftToBlink = m_cursorBlinkInterval;
                    m_pCursor->SetActive(true);
                    const auto cursorPos = m_pTextWidget->GetCursorPosAtEnd();
                    m_pCursor->SetLocalPosition(cursorPos.x, cursorPos.y);

                    break;
                }
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Called when the User finishes typing.
    //-----------------------------------------------------------------------------------------------------------------------------
    void TextFieldWidget::FinishText()
    {
        m_valueEmpty = m_pTextWidget->IsEmpty();

        if (m_valueEmpty)
        {
            m_pTextWidget->SetText(m_defaultText);
        }

        // TODO: This isn't enforced, and could fail... This is a hack
        // Call the OnValueChangedFunction in the ExecuteScript
        else
        {
            lua::CallMemberFunction(m_pOnExecuteScript, "OnValueChanged", m_pTextWidget->GetText().c_str());
            m_defaultText = m_pTextWidget->GetText();
        }

        StopEditing();
    }

    void TextFieldWidget::StopEditing()
    {
        // Deactivate the Cursor.
        m_pCursor->SetActive(false);

        // If our cursor blinks: stop us from updating.
        if (m_cursorBlinkInterval > 0.f)
        {
            GetLayer()->RemoveUpdateable(this);
        }

        // Focus the previous widget.
        m_pUILayer->FocusWidget(m_pPreviousFocused);

        // Check the position of the Mouse and determine if we need to Exit our over or not.
        const auto mousePos = GraphicsManager::Get()->GetWindow()->GetMousePosition();

        if (!PointIntersectsRect(mousePos))
        {
            OnHoverExit();
        }
    }

    bool TextFieldWidget::CanAppendCharacter(const MCPKey key) const
    {
        // Don't append the null character.
        if (key == MCPKey::kNull)
            return false;

        switch (m_constraints)
        {
            case TextInputConstraints::kNone: break;
            case TextInputConstraints::kAlphaOnly:
            {
                return IsAlpha(key);
            }

            case TextInputConstraints::kNumericOnly:
            {
                return IsNumeric(key);
            }

            case TextInputConstraints::kAlphaNumericOnly:
            {
                return IsAlphaNumeric(key);
            }
        }

        return true;
    }


    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Create the background image, the text widget, and the cursor.
    ///		@param data : Data telling us how to create the children.
    //-----------------------------------------------------------------------------------------------------------------------------
    void TextFieldWidget::CreateChildWidgets(const TextFieldData& data)
    {
        WidgetConstructionData constructData;
        constructData.rect.width = m_width;
        constructData.rect.height = m_height;
        constructData.startEnabled = true;
        constructData.sizedToContent = false;

        // Create the Background Image
        auto* pBackgroundImage = BLEACH_NEW(ImageWidget(constructData, data.backgroundImagePath));
        pBackgroundImage->Init();
        AddChild(pBackgroundImage);

        // Create the Text Widget
        constructData.sizedToContent = false;
        constructData.rect.width -= 40.f;        // This should be in data.

        m_pTextWidget = BLEACH_NEW(TextWidget(constructData, m_defaultText.c_str(), data.format));
        m_pTextWidget->Init();
        pBackgroundImage->AddChild(m_pTextWidget);

        // Create the Cursor
        // For now, we are just going to create a TextWidget with a single '|'. In the future this could be an image.
        constructData.sizedToContent = true;
        constructData.startEnabled = false;
        constructData.rect.width = m_width;

        m_pCursor = BLEACH_NEW(TextWidget(constructData, "|", data.format));
        m_pCursor->Init();
        m_pTextWidget->AddChild(m_pCursor);
    }

    const std::string& TextFieldWidget::GetText() const
    {
        return m_pTextWidget->GetText();
    }

    TextFieldWidget* TextFieldWidget::AddFromData(const XMLElement element)
    {
        const auto data = GetWidgetConstructionData(element);
        auto buttonBehavior = GetButtonBehavior(element);

        TextFieldData fieldData;
        fieldData.maxCharCount = element.GetAttributeValue<int>("maxCharacterCount", -1);

        // Background Image
        auto childElement = element.GetChildElement("BackgroundImage");
        if (!childElement.IsValid())
        {
            MCP_ERROR("TextFieldWidget", "Failed to create from data! Failed to find BackgroundImage element!");
            return nullptr;
        }

        fieldData.backgroundImagePath = childElement.GetAttributeValue<const char*>("path");
        MCP_CHECK(fieldData.backgroundImagePath);

        // Default Text
        childElement = childElement.GetSiblingElement("DefaultText");
        if (!childElement.IsValid())
        {
            MCP_ERROR("TextFieldWidget", "Failed to create from data! Failed to find DefaultText element!");
            return nullptr;
        }

        fieldData.defaultText = childElement.GetText();

        // Text Format
        fieldData.format = TextWidget::GetTextFormatData(element);

        // Cursor
        childElement = childElement.GetSiblingElement("Cursor");
        if (childElement.IsValid())
        {
            fieldData.cursorBlinkSpeed = childElement.GetAttributeValue<float>("blinkSpeed", 1.f);
        }

        // Constraints
        childElement = element.GetChildElement("Constraints");
        if (childElement.IsValid())
        {
            const auto label = childElement.GetAttributeValue<const char*>("value", "None");
            fieldData.constraints = static_cast<TextInputConstraints>(HashString32(label));

            // TODO: Have a 'SpacesAllowed' attribute?
        }

        return BLEACH_NEW(TextFieldWidget(data, std::move(buttonBehavior), fieldData));
    }

}
