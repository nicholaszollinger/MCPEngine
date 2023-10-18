---@class ImageWidgetPtr : WidgetPtr

---------------------------------------------------------------------
--- Set the tint of the ImageWidget's texture.
---@param widgetPtr ImageWidgetPtr
---@param color Color
---------------------------------------------------------------------
function Widget.SetTint(widgetPtr, color) end;

---comment
---@param widgetPtr ImageWidgetPtr
---@return ImageWidgetPtr|nil childPtr
function Widget.GetFirstChildImageWidget(widgetPtr)  return nil end;

---comment
---@param widgetPtr ImageWidgetPtr
---@param tag string
---@return ImageWidgetPtr|nil childPtr
function Widget.GetChildImageWidgetByTag(widgetPtr, tag) return nil end