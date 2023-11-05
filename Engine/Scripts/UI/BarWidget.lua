-- BarWidget.lua

---@class BarWidgetPtr : WidgetPtr

---@class BarWidgetLib
---@field GetMax function

---@type BarWidgetLib
BarWidget = {};

-----------------------------------------------------------------------------------------
---Get the max value of this BarWidget.
---@param barWidgetPtr BarWidgetPtr
---@return integer MaxValue
-----------------------------------------------------------------------------------------
function BarWidget.GetMax(barWidgetPtr) return 0; end