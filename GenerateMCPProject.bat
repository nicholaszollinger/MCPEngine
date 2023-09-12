:: Creates a sln file with the engine project already setup. You must pass in a project name.

CALL Tools\Premake\premake5.exe --file="Tools\CreateNewProject.lua" vs2022 %1 %CD%
PAUSE