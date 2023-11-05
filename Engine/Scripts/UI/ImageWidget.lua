---@class ImageWidgetPtr : WidgetPtr

---@class ImageWidgetLib
---@field SetCrop function
---@field GetCrop function
---@field SetTint function

---@type ImageWidgetLib
ImageWidget = {};

---------------------------------------------------------------------
--- USE IMAGEWIDGET.SETTINT() THIS IS BEING REMOVED. Set the tint of the ImageWidget's texture.
---@param widgetPtr ImageWidgetPtr
---@param color Color
---------------------------------------------------------------------
function Widget.SetTint(widgetPtr, color) end;

---------------------------------------------------------------------
---Attempt to get the first ImageWidget child encountered. 
---@param widgetPtr ImageWidgetPtr
---@return ImageWidgetPtr|nil childPtr
---------------------------------------------------------------------
function Widget.GetFirstChildImageWidget(widgetPtr)  return nil end;

---------------------------------------------------------------------
---Attempt to get a ImageWidget pointer of a child.
---@param widgetPtr ImageWidgetPtr
---@param tag string
---@return ImageWidgetPtr|nil childPtr
---------------------------------------------------------------------
function Widget.GetChildImageWidgetByTag(widgetPtr, tag) return nil end

---------------------------------------------------------------------
---Set the crop of an ImageWidget.
---@param imageWidgetPtr ImageWidgetPtr
---@param crop Rect
---------------------------------------------------------------------
function ImageWidget.SetCrop(imageWidgetPtr, crop) end

---------------------------------------------------------------------
---Get the crop of an ImageWidget.
---@param imageWidgetPtr ImageWidgetPtr
---@return Rect|nil
---------------------------------------------------------------------
function ImageWidget.GetCrop(imageWidgetPtr) end

---------------------------------------------------------------------
--- Set the tint of the ImageWidget's texture.
---@param imageWidgetPtr ImageWidgetPtr
---@param color Color
---------------------------------------------------------------------
function ImageWidget.SetTint(imageWidgetPtr, color) end;