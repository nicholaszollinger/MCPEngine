-- Debug.lua
-- Lua 5.4 Reference: http://www.lua.org/manual/5.4/

MCP_DEBUG = {};

-----------------------------------------------------------------------------------------------------------
---Posts a log in debug mode.
---@param category string A Category to give context to the log. This can be a script name for example.
---@param msg string The message to post.
-----------------------------------------------------------------------------------------------------------
function MCP_DEBUG.LOG(category, msg) end;

-----------------------------------------------------------------------------------------------------------
---Posts a warning message in debug mode.
---@param category string A Category to give context to the message. This can be a script name for example.
---@param msg string The message to post.
-----------------------------------------------------------------------------------------------------------
function MCP_DEBUG.WARN(category, msg) end;

-----------------------------------------------------------------------------------------------------------
---Posts an error message in debug mode.
---@param category string A Category to give context to the message. This can be a script name for example.
---@param msg string The message to post.
-----------------------------------------------------------------------------------------------------------
function MCP_DEBUG.ERROR(category, msg) end;

-----------------------------------------------------------------------------------------------------------
---Asserts a condition is true in debug mode.
---@param condition boolean A condition that must be true.
-----------------------------------------------------------------------------------------------------------
function MCP_DEBUG.CHECK(condition) end;
