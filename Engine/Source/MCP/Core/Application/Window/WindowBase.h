#pragma once
// WindowBase.h

#include "Utility/Types/Rect.h"

namespace mcp
{
    class ApplicationEvent;

    class WindowBase
    {
    public:
        WindowBase() = default;
        WindowBase(const WindowBase& other) = delete;
        WindowBase(WindowBase&& right) = delete;
        WindowBase& operator=(const WindowBase& right) = delete;
        WindowBase& operator=(WindowBase&& right) = delete;

        ~WindowBase()
        {
            Close();
        }

        bool Init(const char* pWindowName, const int width, const int height);
        bool ProcessEvents();
        void PostApplicationEvent(ApplicationEvent& event);
        void Close();

        const RectInt& GetDimensions();
        void* GetRenderer();

    private:
        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Get the platform defined WindowType.
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename WindowType>
        WindowType& GetWindow()
        {
            static WindowType window;
            return window;
        }
    };
}