:: REPLACE "_ProjectName_"  with the name of the Project you want to create, 
::         "_Filepath_"     with the file path that you want to use.
SET projectName = %1
SET filePath = %2

CALL Tools\Premake\premake5.exe --file="Tools\CreateNewProject.lua" vs2022 projectName filePath
PAUSE