-- SceneManager.lua

---@class SceneManagerLib
---@field TransitionToScene function # Set active state of the passed in Widget.
---@field PauseWorld function
---@field ResumeWorld function

---@type SceneManagerLib
SceneManager = {};



------------------------------------------------------------------
--- Attempt to queue a transition to a scene matching the Id.
---@param sceneId string Id of the scene you want to transition to.
------------------------------------------------------------------
function SceneManager.TransitionToScene(sceneId) end

------------------------------------------------------------------
--- Pauses the World Layer in the active scene.
------------------------------------------------------------------
function SceneManager.PauseWorld() end

------------------------------------------------------------------
--- Resumes the World Layer in the active scene.
------------------------------------------------------------------
function SceneManager.ResumeWorld() end