-- TextWidget.lua

---@class TextWidgetPtr : WidgetPtr

---@class TextWidgetLib
---@field SetText function 
---@field GetText function 

---@type TextWidgetLib
TextWidget = {};

---------------------------------------------------------------------------------------------
---Try to get a TextWidget in the current Scene.
---@param widgetPtr WidgetPtr
---@param tag string
---@return TextWidgetPtr|nil Result Either the Pointer to the TextWidget, or nil if it failed.
---------------------------------------------------------------------------------------------
function Widget.GetTextWidget(widgetPtr, tag) return nil end

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

---------------------------------------------------------------------------------------------
--- Set the text of a text widget.
---@param textWidgetPtr TextWidgetPtr
---@returns string
---------------------------------------------------------------------------------------------
function TextWidget.GetText(textWidgetPtr) return "" end
