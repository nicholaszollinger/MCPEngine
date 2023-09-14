// Main.cpp
// [Author name here]

#include "MCP/Core/Application/Application.h"

int main()
{
    mcp::Application::Create();
    if (!mcp::Application::Get()->Init("Content/SceneData/DevSceneData.xml"))
    {
        return -1;
    }

    mcp::Application::Get()->Run();
    mcp::Application::Get()->Close();

    mcp::Application::Destroy();

    return 0;
}