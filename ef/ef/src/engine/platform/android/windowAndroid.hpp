#pragma once

#include <src/engine/platform/android/window.hpp>

namespace ef
{
    class AndroidWindow : public Window
    {
    public:
        AndroidWindow();
        virtual ~AndroidWindow() override;

        bool InitDisplay();
        bool InitSurface();
        bool InitContext();

        virtual bool init() override;

        // EGL stuff
        EGLDisplay mEglDisplay;
        EGLSurface mEglSurface;
        EGLContext mEglContext;
        EGLConfig mEglConfig;

        bool HandleEglError(EGLint error);

        virtual void KillGLObjects() override;
        virtual void KillSurface()   override;
        virtual void KillContext()   override;
        virtual void KillDisplay()   override;

        // are our OpenGL objects (textures, etc) currently loaded?
        bool mHasGLObjects;

        bool InitGLObjects();

        uint32_t GetWidth() const override;
        uint32_t GetHeight() const override;

        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        void OnUpdate() override;

        virtual bool getDoesSurfaceExist() override;
        virtual bool getDoesContextExist() override;
        virtual bool getDoesDisplayExist() override;

        virtual void getSurfaceSize(int &w, int &h) override;

        virtual bool swapBuffers() override;

        virtual void log_opengl_error(GLenum err) override;


        void setAndroidApp(android_app *app);

    private:

        // android_app structure
        struct android_app *mApp;

    };
}