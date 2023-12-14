// Resource.cpp

#include "Resource.h"
#include "MCP/Core/Application/Application.h"

namespace mcp
{
    DiskResourceRequest::DiskResourceRequest(const char* path, const char* packagePath, const bool isPersistent)
        : path(Application::Get()->GetContext().workingDirectory + path)
        , packagePath(packagePath)
        , isPersistent(isPersistent)
    {
        //
    }

    DiskResourceRequest::DiskResourceRequest(const std::string& path, const char* packagePath, const bool isPersistent)
        : DiskResourceRequest(path.c_str(), packagePath, isPersistent)
    {
        //
    }

}