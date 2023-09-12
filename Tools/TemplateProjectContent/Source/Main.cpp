// Main.cpp
// [Author name here]

#include "MCP/Application/Application.h"

int main()
{
    mcp::ApplicationProperties props {};
    props.pWindowName = "@{PRJ_NAME}@";
    props.defaultWindowWidth = 1600;
    props.defaultWindowHeight = 900;

    mcp::Application::Create();
    if (!mcp::Application::Get()->Init(props, "Content/SceneData/@{PRJ_NAME}@SceneData.xml"))
    {
        return -1;
    }

    mcp::Application::Get()->Run();
    mcp::Application::Get()->Close();

    mcp::Application::Destroy();

    return 0;
}