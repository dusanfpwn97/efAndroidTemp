#pragma once
#include <stdint.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "game-activity/native_app_glue/android_native_app_glue.h"


#include "our_key_codes.hpp"

namespace ef
{
    class Window
    {
    public:
        //Window();
        virtual ~Window() = default;

        static Window* create(int w, int h);

        //static Window* Create();

        virtual bool init() = 0;
        virtual void OnUpdate() = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void KillGLObjects() = 0;
        virtual void KillSurface()   = 0;
        virtual void KillContext()   = 0;
        virtual void KillDisplay()   = 0;

        virtual bool getDoesSurfaceExist()= 0;
        virtual bool getDoesContextExist()= 0;
        virtual bool getDoesDisplayExist()= 0;

        virtual void getSurfaceSize(int &w, int &h)= 0;

        virtual bool swapBuffers()= 0;

        virtual void log_opengl_error(GLenum err)= 0;


    };
}