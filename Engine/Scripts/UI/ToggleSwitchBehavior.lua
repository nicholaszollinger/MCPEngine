-- ToggleSwitchBehavior.lua
-----------------------------------------------------------------------------------------
-- This is basic behavior for a Toggle Switch.
-- When the toggle button is executed, we need to move the toggle button image's anchor 
-- to match the active state of the toggle.
-----------------------------------------------------------------------------------------

local m = ButtonWidgetScript:New();
m.owner = nil;
m.knobOffset = 0.0;
m.knobImageWidget = nil;
m.trayImageWidget = nil;
m.anchorOff = {x=0, y=0.5};
m.anchorOn = {x=1, y=0.5};
m.defaultKnobColor = {r = 200, g = 200, b = 200, alpha = 255};
m.hoveredKnobColor = {r = 255, g = 255, b = 255, alpha = 255};
m.defaultTrayColor = {r = 80, g = 80, b = 80, alpha = 255};
m.hoveredTrayColor = {r = 120, g = 120, b = 120, alpha = 255};

function m.SetData(data)
    m.anchorOff = data.anchorOff;
    m.anchorOn = data.anchorOn;
    m.defaultKnobColor = data.defaultKnobColor;
    m.defaultTrayColor = data.defaultTrayColor;
    m.hoveredKnobColor = data.hoveredKnobColor;
    m.hoveredTrayColor = data.hoveredTrayColor;
end

function m.Init(widgetPtr, isActive)
    m.owner = widgetPtr;

    -- Get the child background widget. This is the first image widget we find.
    m.trayImageWidget = Widget.GetFirstChildImageWidget(widgetPtr);
    if m.trayImageWidget == nil then
        MCP_DEBUG.ERROR("ToggleSwitch", "Failed to get background image!");
        return;
    end

    -- Get the check image widget. This is the first child image widget of the background we find.
    m.knobImageWidget = Widget.GetFirstChildImageWidget(m.trayImageWidget);
    if (m.knobImageWidget == nil) then
        MCP_DEBUG.ERROR("ToggleSwitch", "Failed to find checkbox image widget!");
    end
    
    local width, _ = Widget.GetDimensions(m.knobImageWidget);
    m.knobOffset = width / 2.0;

    m.OnHoverExit();
    m.OnExecute(isActive);
end

-----------------------------------------------------------------------------------------
---Move the toggle handle to the side based on the active state of the toggle.
---@param isActive boolean The active state of the toggle.
-----------------------------------------------------------------------------------------
function m.OnExecute(isActive)
    if isActive == true then
        Widget.SetAnchor(m.knobImageWidget, m.anchorOn.x, m.anchorOn.y);
        Widget.SetPosition(m.knobImageWidget, -m.knobOffset, 0.0);
    else
        Widget.SetAnchor(m.knobImageWidget, m.anchorOff.x, m.anchorOff.y);
        Widget.SetPosition(m.knobImageWidget, m.knobOffset, 0.0);
    end
end

function m.OnHoverEnter()
    --MCP_DEBUG.LOG("Checkbox", "Entering button...");
    Widget.SetTint(m.trayImageWidget, m.hoveredTrayColor);
    Widget.SetTint(m.knobImageWidget, m.hoveredKnobColor);
end

function m.OnHoverExit()
    Widget.SetTint(m.trayImageWidget, m.defaultTrayColor);
    Widget.SetTint(m.knobImageWidget, m.defaultKnobColor);
end

return m;