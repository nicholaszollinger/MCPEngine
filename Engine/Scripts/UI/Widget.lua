-- Widget.lua

require("Engine.Scripts.Core.Class")

---@class WidgetPtr Pointer to a C++ Widget Object.

---@class WidgetScript
---@field owner WidgetPtr Pointer to the C++ Widget object that owns this script.
---@field ScriptData table # Any data that is necessary for the script to work.
---@field OnEnable function # Function called when the widget is activated.
---@field OnDisable function # Function called when the widget is deactivated.
---@field OnHoverEnter function # Function called when the widget has input focus.
---@field OnHoverExit function # Function called when the widget loses input focus.

WidgetScript = CreateClass();
WidgetScript.owner = nil;
WidgetScript.ScriptData = {};

--------------------------------------------------------------------------------
--- Set the Widget script's owner.
---@param widgetPtr WidgetPtr
--------------------------------------------------------------------------------
function WidgetScript.Init(widgetPtr)
    WidgetScript.owner = widgetPtr;
end

function WidgetScript.OnEnable() end;
function WidgetScript.OnDisable() end;
function WidgetScript.OnHoverEnter() end;
function WidgetScript.OnHoverExit() end;

---@class WidgetLib
---@field SetActive function # Set active state of the passed in Widget.
---@field Focus function # Focuses a Widget, allowing it to respond to input events.
---@field SetTint function
---@field GetFirstChildImageWidget function
---@field GetChildImageWidgetByTag function
---@field GetCanvasWidget function
---@field GetChildCanvasWidgetByTag function
---@field SetAnchor function
---@field GetDimensions function
---@field SetPosition function
---@field GetLocalPosition function
---@field GetTextWidget function
---@field GetFirstChildTextWidget function
---@field AddToLayerStack function
---@field PopLayerStack function
---@field SetWidth function
---@field FindImageWidgetByTag function

---@type WidgetLib
Widget = {};

--------------------------------------------------------------------------------
--- Set active state of the passed in Widget.
---@param WidgetPtr WidgetPtr Widget we are setting the active state for.
---@param isActive boolean Active or not.
--------------------------------------------------------------------------------
function Widget.SetActive(WidgetPtr, isActive) end

--------------------------------------------------------------------------------
--- Focuses a Widget, allowing it to respond to input events.
---@param WidgetPtr WidgetPtr Widget we are setting focused.
--------------------------------------------------------------------------------
function Widget.Focus(WidgetPtr) print("Wrong implementation..."); end

----------------------------------------------------------------------------------------------------
--- Set the Anchor of a Widget.
---@param widgetPtr WidgetPtr
---@param anchorX number Value between [0,1]. 0 is the minimum of the axis, and 1 is the max.
---@param anchorY number Value between [0,1]. 0 is the minimum of the axis, and 1 is the max.
----------------------------------------------------------------------------------------------------
function Widget.SetAnchor(widgetPtr, anchorX, anchorY) end

----------------------------------------------------------------------------------------------------
---Returns the width and height of the widget's rect.
---@param widgetPtr WidgetPtr
---@return number width, number height
----------------------------------------------------------------------------------------------------
function Widget.GetDimensions(widgetPtr) return 0,0; end

----------------------------------------------------------------------------------------------------
--- Set the position of a widget.
---@param widgetPtr WidgetPtr
---@param x number
---@param y number
----------------------------------------------------------------------------------------------------
function Widget.SetPosition(widgetPtr, x, y) end;

----------------------------------------------------------------------------------------------------
---Get the Local position of the Widget.
---@param widgetPtr WidgetPtr
---@return integer x
---@return integer y
----------------------------------------------------------------------------------------------------
function Widget.GetLocalPosition(widgetPtr) return 0,0; end;

----------------------------------------------------------------------------------------------------
--- Add the passed in Widget to the UI Layer's widget stack. This can be used to open a new menu.
---@param widgetPtr WidgetPtr
----------------------------------------------------------------------------------------------------
function Widget.AddToLayerStack(widgetPtr) end;

----------------------------------------------------------------------------------------------------
--- Pop the top widget off of the UI Layer's widget stack. This can be used to go back to a previous
--- menu.
---@param widgetPtr WidgetPtr
----------------------------------------------------------------------------------------------------
function Widget.PopLayerStack(widgetPtr) end;

----------------------------------------------------------------------------------------------------
--- Set the widget's width.
---@param widgetPtr WidgetPtr
---@param width number
----------------------------------------------------------------------------------------------------
function Widget.SetWidth(widgetPtr, width) end;
