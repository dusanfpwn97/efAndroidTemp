#pragma once

#include <Window.hpp>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "game-activity/native_app_glue/android_native_app_glue.h"

#include "our_key_codes.hpp"

namespace ef
{
    class AndroidWindow : public Window
    {
    public:
        AndroidWindow();
        ~AndroidWindow() override;

        bool createWindow(int w, int h) override;

        bool InitDisplay();
        bool InitSurface();
        bool InitContext();

        void setAndroidApp(android_app *app);


        // EGL stuff
        EGLDisplay mEglDisplay;
        EGLSurface mEglSurface;
        EGLContext mEglContext;
        EGLConfig mEglConfig;


        // android_app structure
        struct android_app *mApp;

        bool HandleEglError(EGLint error);

        void KillGLObjects();
        void KillSurface();
        void KillContext();
        void KillDisplay();

        // are our OpenGL objects (textures, etc) currently loaded?
        bool mHasGLObjects;

        bool InitGLObjects();


        void log_opengl_error(GLenum err);

        uint32_t GetWidth() const override;
        uint32_t GetHeight() const override;

        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        void OnUpdate() override;

    };
}