-- ToggleCheckboxBehavior.lua
-----------------------------------------------------------------------------------------
-- This is basic behavior for a Checkbox Toggle.
-- When the Toggle button is executed, we are setting the 'Check' ImageWidget's active state based on the
-- value of the toggle.
-----------------------------------------------------------------------------------------

local m = ButtonWidgetScript:New();
m.owner = nil;
m.checkImageWidget = nil;
m.backgroundImageWidget = nil;
m.defaultCheckColor = {r = 200, g = 200, b = 200, alpha = 255};
m.hoveredCheckColor = {r = 255, g = 255, b = 255, alpha = 255};
m.defaultBackgroundColor = {r = 80, g = 80, b = 80, alpha = 255};
m.hoveredBackgroundColor = {r = 120, g = 120, b = 120, alpha = 255};

function m.SetData(data)
    m.defaultCheckColor = data.defaultCheckColor;
    m.hoveredCheckColor = data.hoveredCheckColor;
    m.defaultBackgroundColor = data.defaultBackgroundColor;
    m.hoveredBackgroundColor = data.hoveredBackgroundColor;
end

function m.Init(widgetPtr, isActive)
    m.owner = widgetPtr;

    --MCP_DEBUG.LOG("Checkbox", "Initializing CheckBoxScript");
    -- Get the child background widget. This is the first image widget we find.
    m.backgroundImageWidget  = Widget.GetFirstChildImageWidget(widgetPtr);
    if m.backgroundImageWidget == nil then
        MCP_DEBUG.ERROR("Checkbox", "Failed to get background image!");
        return;
    end

    -- Get the check image widget. This is the first child image widget of the background we find.
    m.checkImageWidget = Widget.GetFirstChildImageWidget(m.backgroundImageWidget);
    if (m.checkImageWidget == nil) then
        MCP_DEBUG.ERROR("Checkbox", "Failed to find checkbox image widget!");
    end

    -- Set the default colors.
    m.OnHoverExit();
    Widget.SetActive(m.checkImageWidget, isActive);
end

-----------------------------------------------------------------------------------------
---Set the check image active or not based on the state of the toggle.
---@param isActive boolean The active state of the toggle.
-----------------------------------------------------------------------------------------
function m.OnExecute(isActive)
    Widget.SetActive(m.checkImageWidget, isActive);
end

function m.OnHoverEnter()
    --MCP_DEBUG.LOG("Checkbox", "Entering button...");
    Widget.SetTint(m.backgroundImageWidget, m.hoveredBackgroundColor);
    Widget.SetTint(m.checkImageWidget, m.hoveredCheckColor);
end

function m.OnHoverExit()
    Widget.SetTint(m.backgroundImageWidget, m.defaultBackgroundColor);
    Widget.SetTint(m.checkImageWidget, m.defaultCheckColor);
end

return m;