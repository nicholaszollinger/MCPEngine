#pragma once
// EntryPoint.h

#include "Config.h"
#include "Application/Application.h"

// TODO:
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