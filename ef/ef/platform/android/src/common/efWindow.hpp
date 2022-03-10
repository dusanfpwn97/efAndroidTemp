#pragma once

namespace ef
{
    class efWindow
    {
    public:
        efWindow();
        virtual ~efWindow();

        virtual bool createWindow(int w, int h);
        void configureOpenGL();

        bool PrepareToRender();

    protected:

        virtual bool InitDisplay();
        virtual bool InitSurface();
        virtual bool InitContext();

    };
}