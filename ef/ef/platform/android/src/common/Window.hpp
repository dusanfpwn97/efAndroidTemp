#pragma once
#include <stdint.h>


namespace ef
{
    class Window
    {
    public:
        //Window();
        virtual ~Window() = default;

        virtual bool createWindow(int w, int h) = 0;

        //static Window* Create();

        //virtual void initt() = 0;
        virtual void OnUpdate() = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;


    };
}