-- SliderWidgetBehavior.lua
-----------------------------------------------------------------------------------------
-- This is basic behavior for a Slider.
-- There is a knob and a slider value, and clicking on the slider moves the knob.
-----------------------------------------------------------------------------------------

local m = ButtonWidgetScript:New();
m.knobOffset = 0.0;
m.defaultKnobColor = {r = 200, g = 200, b = 200, alpha = 255};
m.hoveredKnobColor = {r = 255, g = 255, b = 255, alpha = 255};
m.defaultSliderGrooveColor = {r = 80, g = 80, b = 80, alpha = 255};
m.hoveredSliderGrooveColor = {r = 120, g = 120, b = 120, alpha = 255};

function m.SetData(data)
    m.defaultKnobColor = data.defaultKnobColor;
    m.hoveredKnobColor = data.hoveredKnobColor;
    m.defaultSliderGrooveColor = data.defaultSliderGrooveColor;
    m.hoveredSliderGrooveColor = data.hoveredSliderGrooveColor;
end

function m.Init(widgetPtr, sliderPercentage)
    m.owner = widgetPtr;

    -- Get the child background widget. This is the first image widget we find.
    m.sliderGrooveImageWidget = Widget.GetFirstChildImageWidget(widgetPtr);
    if m.sliderGrooveImageWidget == nil then
        MCP_DEBUG.ERROR("SliderWidget", "Failed to get background image!");
        return;
    end

    -- Get the check image widget. This is the first child image widget of the background we find.
    m.knobImageWidget = Widget.GetFirstChildImageWidget(m.sliderGrooveImageWidget);
    if (m.knobImageWidget == nil) then
        MCP_DEBUG.ERROR("SliderWidget", "Failed to find checkbox image widget!");
    end

    m.OnExecute(sliderPercentage)
    m.OnHoverExit();
end

function m.OnExecute(sliderPercentage)
    -- Move the knob's anchor based on the percentage
    Widget.SetAnchor(m.knobImageWidget, sliderPercentage, 0.5);
end

function m.OnHoverEnter()
    --MCP_DEBUG.LOG("Checkbox", "Entering button...");
    Widget.SetTint(m.sliderGrooveImageWidget, m.hoveredSliderGrooveColor);
    Widget.SetTint(m.knobImageWidget, m.hoveredKnobColor);
end

function m.OnHoverExit()
    Widget.SetTint(m.sliderGrooveImageWidget, m.defaultSliderGrooveColor);
    Widget.SetTint(m.knobImageWidget, m.defaultKnobColor);
end

return m;