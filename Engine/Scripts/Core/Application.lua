-- Application.lua

---@class ApplicationLib
---@field QuitGame function # Set active state of the passed in Widget.
---@field SetLanguage function

---@type ApplicationLib
Application = {};

------------------------------------------------------------------
--- Quits the Application.
------------------------------------------------------------------
function Application.QuitGame() end

------------------------------------------------------------------
--- Sets the Language based on the token
---@param token string Name of the language.
------------------------------------------------------------------
function Application.SetLanguage(token) end