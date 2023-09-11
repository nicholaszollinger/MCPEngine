-- CreateNewProject

-- PARAMS:
    -- _ARGS[1] = ProjectName
    -- _ARGS[2] = Path
    -- TODO: Maybe a Branch name? So that we could reuse branches?
local NewProjectName = _ARGS[1];
local NewSolutionRoot = "../" .. _ARGS[2] .. NewProjectName;

--local LauncherRoot = path.getabsolute("../");
--local EngineVersionsFolder = path.getabsolute("../MCP/Versions/");
local EngineRepoURL = "https://github.com/nicholaszollinger/MCPEngine.git";

-- GET THE MAIN BRANCH FOR THE ENGINE.
-- Whenever we create a new project, I want to create a new branch
-- for project that is based on the main branch of the engine.
-- This way, when the engine evolves for that specific project, it 
-- won't effect the other projects.

-- Create the Module for building the engine.
local ModuleName = "MCP Project Generator";
premake.modules.mcp_build_generator = {};
local m = premake.modules.mcp_build_generator;
m.DEBUG = true;

-- Project Name option.
-- newoption
-- {
--     trigger = "projectName",
--     description = "The name of the project to be generated";
-- }

-- -- Project Location option.
-- newoption
-- {
--     trigger = "projectLocation",
--     description = "Where the new project will live.";
-- }

function m.Initialize()
    m._tokens = {}
    -- m.projectName = m._defaultOrOption("projectName", m.GetDefaultProjectName());
    -- m.projectLocation = m._defaultOrOption("projectLocation", )

    -- Create the folder structure that we want.
    m.solutionRoot = NewSolutionRoot;
    m.projectRoot = NewSolutionRoot .. "/" .. NewProjectName .. "/";
    m.engineRoot = NewSolutionRoot .."/MCPEngine/";
    os.mkdir(m.solutionRoot);
    os.mkdir(m.projectRoot);
    os.mkdir(m.engineRoot);
end

--- Creates an engine folder where the 
function m.CloneRepoAndCreateBranch()
    
    os.chdir(m.solutionRoot);

    -- Clone the main branch
    os.execute("git clone " .. EngineRepoURL);
    
    -- Create a new branch of the repo with the title of the new project
    os.chdir(m.engineRoot);
    os.execute("git checkout -b " .. NewProjectName);

    os.execute("git push -u origin" .. NewProjectName);

    -- Change our directory back to the solution root.
    os.chdir(m.solutionRoot);
end

--- TODO: Create the Default project files for a new MCP project.
function m.GenerateProjectFiles()

end


--- Tokens are stand in string values that are found in the names of the boilerplate
--- code. Example: 
    --- - Boilerplate file name: "@{PRJ_NAME}@App.h".
    --- - The token name is "PRJ_NAME"
    --- - A replacement val could be "HelloWorld"
    --- - The final file name will be "HelloWorldApp.h"
--- @param token string | "The text to be replaced."
--- @param replacement string | "The text to replace the token." 
function m.AddToken(token, replacement)

    -- If we haven't made our table of tokens, return.
    if m._tokens == nil then
        return false;
    end

    -- If we already have a token by that name, then return false.
    if m._tokens[token] ~= nil then
        m.PrintDebug("Already had token with the name: " .. token);
        return false;
    end

    local replacementType = type(replacement);

    if replacementType ~= "string" and replacementType ~= "function" then
        m.PrintDebug("Token `" .. token .. "` replacement is not of type string or function. Type is : " .. replacementType);
        return false;
    end

    m.PrintDebug("Adding token: " .. token);
    m._tokens[token] = replacement;

end

--- Print out some debug information for this module.
function m.PrintDebug(...)
    if m.DEBUG == false then
        return;
    end

    print("[" .. ModuleName .. "] ")
    print(...);
end 

function m.PrintInfo(...)
    premake.info("[" .. ModuleName .. "] ");
    premake.info(...);
end

-- PROGRAM STARTS HERE
m.Initialize();
m.CloneRepoAndCreateBranch();
local ObjDirectoryPath = "!$(SolutionDir)Build/Intermediate/$(ProjectName)/$(Configuration)/$(PlatformTarget)/";

-- CREATE THE NEW SOLUTION
-- These are Premake functions.
workspace(NewProjectName);
    configurations {"Debug", "Release"}
    platforms {"x64"} -- I don't build for x86
    location(m.solutionRoot)
    startproject(NewProjectName);

    -- Solution-wide Debug Configuration settings
    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"

    -- Solution-wide Release Configuration settings
    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"

    -- Platform Setup.
    filter { "platforms:x64"}
        system "Windows"
        architecture "x64"

-- ENGINE PROJECTS
group "Engine"
    externalproject("Utility")
        location(m.engineRoot .. "Utility/")
        uuid("b61061c3-ac16-458d-9348-cd7a73565573") -- TODO: Look this up
        kind "StaticLib"
        language "C++"

    externalproject("MCP")
        location (m.engineRoot .. "MCP/")
        uuid("0a2bae05-3362-489b-902d-2b9015220220")
        kind "StaticLib"
        language "C++"
        links {"Utility"}

-- GAME PROJECT
group "Game"
    project(NewProjectName)
        location(m.projectRoot);
        kind "ConsoleApp"
        language "C++"
        targetdir("$(SolutionDir)Build/Bin/$(PlatformTarget)/$(Configuration)/");
        objdir(ObjDirectoryPath);
        links {"MCP", "Utility"}
        cppdialect "C++17"
        warnings "Extra"
        libdirs {"$(SolutionDir)Build/Lib/MCP/$(PlatformTarget)/$(Configuration)/"}
        includedirs 
        {
            "../Utility/Source/", -- Utility Source
            "../MCP/Source/", -- Engine Source
        };

        -- System SDK version.
        filter "system:windows"
            systemversion "latest"
        
        -- -- Copy over the boilerplate code immediately, so that we can include the files in the project.
        -- os.execute("xCopyToDir.bat \"Game Boilerplate Source\" \"" .. GameRootFolder .. "/Source/\"");
        
        -- -- This will add all of the source files that have been copied into our source folder into the project.
        -- files 
        -- {   m.projectRoot .. "/Source/" .. "**.h", 
        --     m.projectRoot .. "/Source/" .. "**.cpp",
        --     m.projectRoot .. "/Source/" .. "**.ixx"
        -- };
