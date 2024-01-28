-- ToggleWidget
---@class ToggleWidgetPtr : WidgetPtr

---@class ToggleWidgetLib
---@field SetValue function
---@field GetValue function

---@type ToggleWidgetLib
ToggleWidget = {};

function ToggleWidget.SetValue(ToggleWidgetPtr, enabled) end;
function ToggleWidget.GetValue(ToggleWidgetPtr) end;