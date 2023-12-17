#pragma once
// EntryPoint.h

#include "Config.h"
#include "Application/Application.h"

//--------------------------------------------------------------------------------------------------------------------------
//      NOTES:
//      This is an interesting idea where I would define the main function for the game project; they would just have to
//      include this file, and define certain functions like creating the Application.
//
//      I like this approach because I need to some specific things in the main function, but I didn't have time to make this
//      work. Something for the future. Maybe.
//
//--------------------------------------------------------------------------------------------------------------------------

//namespace mcp
//{
//    extern mcp::Application* CreateApp();
//
//#ifdef MCP_PLATFORM_WINDOWS
//    int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
//    {
//        auto* pApp = CreateApp();
//        Application::Create();
//
//        if (!pApp->Init("Content/SceneData/UIClassSceneData.xml"))
//        {
//            return -1;
//        }
//
//        pApp->Run();
//
//        if (!mcp::Application::Get()->Init("Content/SceneData/UIClassSceneData.xml"))
//        {
//            return -1;
//        }
//
//        mcp::Application::Get()->Run();
//        mcp::Application::Destroy();
//
//
//        return 0;
//    }
//#endif
//}