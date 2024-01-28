-- Debug.lua
-- Lua 5.4 Reference: http://www.lua.org/manual/5.4/

-- TODO: Register this module with some sort of container?
-- mcp.Modules.Debug = {};

local m = {};
m.DEBUG = false; -- TODO: Should this be set here? This should be set by the Engine in C++

------------------------------------------------------------------------------------------------------------
---Initialize a module with the MCP debug functions.
------------------------------------------------------------------------------------------------------------
function m.Init()
    -- If we are in debug mode, then we will register the debug functions.
    if m.DEBUG == true then
        m.ASSERT = function(condition, msg) ASSERT(condition, msg); end;
        m.LOG = function(module, msg) LOG(module, msg); end;
        m.WARN = function(module, msg) WARN(module, msg); end;
        m.ERROR = function(module, msg) ERROR(module, msg); end;

    -- Otherwise, we are going to set them to empty functions.
    else
        m.ASSERT = function(_, _) end;
        m.LOG = function(_, _) end;
        m.WARN = function(_, _) end;
        m.ERROR = function(_, _) end;
    end
end

------------------------------------------------------------------------------------------------------------
---Assert that a condition is true. If the condition is false, then we post an assertion from the Engine. 
--- - ONLY RUNS IF DEBUG flag is set.
---@param condition boolean The condition you are asserting.
---@param msg string An optional message to accompany an assertion failed message.
------------------------------------------------------------------------------------------------------------
function ASSERT(condition, msg)
    if condition == true then
        return;
    end

    -- Get the debug info from the function that called this function.
    -- 0 is this function, 1 is the function that called assert (this module calls into this), 2 is the module that called the debug func.
    local debugInfo = debug.getinfo(2);

    -- Call the Appropriate MCP assert function.
    if msg == nil then
        MCPAssert("Lua Assertion Failed!", "\n\tFile: " .. debugInfo.source .. " - " .. debugInfo.func .. "() - Line: " .. debugInfo.currentline);

    else
        MCPAssertMsg("Lua Assertion Failed!", msg .. "\n\tFile: " .. debugInfo.source .. " - " .. debugInfo.func .. "() - Line: " .. debugInfo.currentline);
    end
end

------------------------------------------------------------------------------------------------------------
---Log a message.
---@param module table Module that is calling the log function.
---@param msg string Message that you want to log.
------------------------------------------------------------------------------------------------------------
function LOG(module, msg)
    if m.DEBUG == false then
        return;
    end

    -- I am assuming that the module has a moduleName field.
    MCPLog(module.moduleName, msg);
end

------------------------------------------------------------------------------------------------------------
---Log a warning message.
---@param module table Module that is calling the log function.
---@param msg string Message that you want to log.
------------------------------------------------------------------------------------------------------------
function WARN(module, msg)
    if m.DEBUG == false then
        return;
    end

    -- I am assuming that the module has a moduleName field.
    MCPWarn(module.moduleName, msg);
end

------------------------------------------------------------------------------------------------------------
---Log an error message.
---@param module table Module that is calling the log function.
---@param msg string Message that you want to log.
------------------------------------------------------------------------------------------------------------
function ERROR(module, msg)
    if m.DEBUG == false then
        return;
    end

    -- I am assuming that the module has a moduleName field.
    MCPError(module.moduleName, msg);
end

return m;
