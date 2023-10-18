-- TextWidget.lua

---@class TextWidgetPtr : WidgetPtr

---@class TextWidgetLib
---@field SetText function 

---@type TextWidgetLib
TextWidget = {};

---------------------------------------------------------------------------------------------
--- Get a pointer to the first encountered child that is a TextWidget.
---@param widgetPtr WidgetPtr
---@return TextWidgetPtr|nil
---------------------------------------------------------------------------------------------
function Widget.GetFirstChildTextWidget(widgetPtr) end;

---------------------------------------------------------------------------------------------
--- Set the text of a text widget.
---@param textWidgetPtr TextWidgetPtr
---@param text string
---------------------------------------------------------------------------------------------
function TextWidget.SetText(textWidgetPtr, text) end