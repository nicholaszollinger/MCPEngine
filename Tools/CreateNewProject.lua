-- CreateNewProject.lua
-- Lua Documentation: https://premake.github.io/docs/

-- PARAMS:
    -- _ARGS[1] = ProjectName
    -- _ARGS[2] = Path, the current directory of where the .bat file was called from.
    -- TODO: Maybe a Branch name? So that we could reuse branches?
local NewProjectName = _ARGS[1];
local NewSolutionRoot = _ARGS[2];

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

-----------------------------------------------------------------------------------
-- Initialize the module variables.
-----------------------------------------------------------------------------------
function m.Initialize()
    m._tokens = {}
    -- m.projectName = m._defaultOrOption("projectName", m.GetDefaultProjectName());
    -- m.projectLocation = m._defaultOrOption("projectLocation", )

    -- Module variables for main root folders.
    m.solutionRoot = NewSolutionRoot;
    m.projectRoot = NewSolutionRoot .. "/" .. NewProjectName .. "/";
    m.engineRoot = NewSolutionRoot .."/MCPEngine/";
    --os.mkdir(m.solutionRoot);

    -- Make the Project folder structure.
    -- os.mkdir(m.projectRoot);
    --os.mkdir(m.projectRoot .. "/Source/");
    --os.mkdir(m.projectRoot .. "/Assets/");
    -- TODO: Should I make the log folders as well?
end

-----------------------------------------------------------------------------------
--- Checkout a new branch based on the NewProjectName
-----------------------------------------------------------------------------------
function m.CreateBranch()
    
    --os.chdir(m.solutionRoot);

    -- Clone the main branch
    --os.execute("git clone " .. EngineRepoURL);
    
    -- Create a new branch of the repo with the title of the new project
    --os.chdir(m.engineRoot);
    --os.execute("git checkout -b " .. NewProjectName);

    -- TODO Figure out what this is doing.
    --os.execute("git push -u origin " .. NewProjectName);

    -- Change our directory back to the solution root.
    --os.chdir(m.solutionRoot);
end

-----------------------------------------------------------------------------------
--- TODO: Create the Default project files for a new MCP project.
-----------------------------------------------------------------------------------
function m.GenerateDefaultFiles()
    -- Create tokens used in the boilerplate code
    m.AddToken("PRJ_NAME", NewProjectName);
    --m.listTokens();

    -- Get the directories in the TemplateProjectContent.
    local dirs = os.matchdirs("TemplateProjectContent/**");
    for _, match in pairs(dirs) do
        local newDirName = m._replaceTokens(match);
        newDirName = string.gsub(newDirName, "TemplateProjectContent", m.solutionRoot);
        local success = os.mkdir(newDirName);

        if success then
            print("Making Dir: " .. newDirName);
        else
            print("Failed to make Dir: " .. newDirName);
        end

    end
    
    -- Translate and write all of the template files as well.
    local files = os.matchfiles("TemplateProjectContent/**.*");

    -- For each template file, we need to 
    for _, match in pairs(files) do

        -- Set the correct filename, with the updated path.
        local newFilename = m._replaceTokens(match);
        newFilename = string.gsub(newFilename, "TemplateProjectContent", m.solutionRoot);

        -- Read the file and replace any tokens
        local fileData = m._readFile(match);
        local result = m._replaceTokens(fileData);

        -- Write the new file to the project location.
        local writeSuccess = io.writefile(newFilename, result);

        -- Check for success.
        if writeSuccess then 
            print("Writing file: " .. newFilename);
        else
            print("Failed to write file: " .. newFilename);
        end

    end

end

-----------------------------------------------------------------------------------
--- Tokens are stand in string values that are found in the names of the boilerplate
--- code. Example: 
    --- - Boilerplate file name: "@{PRJ_NAME}@App.h".
    --- - The token name is "PRJ_NAME"
    --- - A replacement val could be "HelloWorld"
    --- - The final file name will be "HelloWorldApp.h"
--- @param token string | "The text to be replaced."
--- @param replacement string | "The text to replace the token."
----------------------------------------------------------------------------------- 
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

    --m.PrintDebug("Adding token: " .. token);
    m._tokens[token] = replacement;
end

-----------------------------------------------------------------------------------
-- Print out all of the tokens currently registered with the module.
-- Used for debugging
-----------------------------------------------------------------------------------
function m.listTokens()
    if m._tokens ~= nil then
        for token, replacement in pairs(m._tokens) do
            print("\t- " .. token .. ": ");
            local replacementType = type(replacement);

            if replacementType == "string" then
                print(replacement);
            else
                print(replacementType);
            end
        end
    end
end

-----------------------------------------------------------------------------------
-- Read an entire file into a string, and return the result.
-- @param filename : "Name of the file to read."
-----------------------------------------------------------------------------------
function m._readFile(filename)
    local f = assert(io.open(filename, "rb"));
    local content = f:read("*all");
    f:close();
    return content;
end

-----------------------------------------------------------------------------------
-- Get the replacement part for the token.
-----------------------------------------------------------------------------------
function m._getTokenReplacement(token)
    local replacement = m._tokens[token] or nil;

    if replacement ~= nil then
        local replacementType = type(replacement);

        if replacementType == "string" then
            return replacement;
        else
            return nil;
        end

    else
        print("Found token but failed to find replacement. Token: " .. token);
    end
end

-----------------------------------------------------------------------------------
-- Replace all tokens and replace them in the string.
-- @returns : "A string containing the replaced values."
-----------------------------------------------------------------------------------
function m._replaceTokens(string)
    -- Function to grab the token.
    local replacementFunc = function(token)
        local result = m._getTokenReplacement(token);
        return result;
        end

    -- Find every token matching the following pattern
    local result, num = string.gsub(string, "@{(.-)}@", replacementFunc);

    -- Debugging func:
    if num ~= 0 then
        print("Substitutions made: ", num);
    end

    return result;
end

-----------------------------------------------------------------------------------
--- Print out some debug information for this module.
-----------------------------------------------------------------------------------
function m.PrintDebug(...)
    if m.DEBUG == false then
        return;
    end

    print("[" .. ModuleName .. "] ")
    print(...);
end 

-----------------------------------------------------------------------------------
function m.PrintInfo(...)
    premake.info("[" .. ModuleName .. "] ");
    premake.info(...);
end
-----------------------------------------------------------------------------------

-----------------------------------------------------------------------------------
-- PROGRAM EXECUTION STARTS HERE
-----------------------------------------------------------------------------------

m.Initialize();
m.CreateBranch();

-- VS Variables.
local ObjDirectoryPath = "!$(SolutionDir)Build/Intermediate/$(ProjectName)/$(Configuration)/$(PlatformTarget)/";
local UtilitySourcePath = "$(SolutionDir)MCPEngine/Utility/Source/";
local EngineSourcePath = "$(SolutionDir)MCPEngine/Engine/Source/";

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
        uuid("b61061c3-ac16-458d-9348-cd7a73565573")
        kind "StaticLib"
        language "C++"

    externalproject("MCPEngine")
        location (m.engineRoot .. "Engine/")
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
        links {"MCPEngine", "Utility"}
        cppdialect "C++17"
        warnings "Extra"
        libdirs {"$(SolutionDir)Build/Lib/MCPEngine/$(PlatformTarget)/$(Configuration)/"}
        includedirs 
        {
            UtilitySourcePath,
            EngineSourcePath,
        };

        dependenciesfile("MCPEngine.lib")

        -- System SDK version.
        filter "system:windows"
            systemversion "latest"
        
        -- GENERATE DEFAULT ASSETS
        m.GenerateDefaultFiles();

        -- -- Copy over the boilerplate code immediately, so that we can include the files in the project.
        -- os.execute("xCopyToDir.bat \"Game Boilerplate Source\" \"" .. GameRootFolder .. "/Source/\"");
        
        -- This will add all of the source files that have been copied into our source folder into the project.
        files 
        {   m.projectRoot .. "/Source/" .. "**.h",
            m.projectRoot .. "/Source/" .. "**.cpp",
            m.projectRoot .. "/Source/" .. "**.ixx"
        };


