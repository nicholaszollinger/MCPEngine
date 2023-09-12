#pragma once
// Process.h

namespace mcp
{
    class IProcess
    {
    public:
        virtual ~IProcess() = default;

        virtual bool Init() = 0;
        virtual void Close() = 0;
    };
}