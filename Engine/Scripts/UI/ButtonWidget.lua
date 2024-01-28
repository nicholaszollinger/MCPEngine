-- ButtonWidget.lua
require("Engine.Scripts.UI.Widget")

---@class ButtonWidgetScript: WidgetScript
---@field OnExecute function # Function called when the button is clicked.
---@field OnPress function # Function called when the button is pressed, not executed.
---@field OnRelease function # Function called when the button is released.

ButtonWidgetScript = CreateClass(WidgetScript)

--- Function called when the button is clicked.
function ButtonWidgetScript.OnExecute() end;
function ButtonWidgetScript.OnPress() end;
function ButtonWidgetScript.OnRelease() end;