//
// Created by dusan on 05-Mar-22.
//

#include "WindowAndroid.hpp"
#include <android/window.h>
#include "scene_manager.hpp"

namespace ef
{

    AndroidWindow::AndroidWindow() {
        mEglDisplay = EGL_NO_DISPLAY;
        mEglSurface = EGL_NO_SURFACE;
        mEglContext = EGL_NO_CONTEXT;
        mEglConfig = 0;

        mHasGLObjects = false;


        init();

    }

    AndroidWindow::~AndroidWindow() {
        KillContext();
    }

    bool AndroidWindow::InitDisplay() {
        if (mEglDisplay != EGL_NO_DISPLAY) {
            // nothing to do
            //ALOGI("NativeEngine: no need to init display (already had one).");
            return true;
        }

        //ALOGI("NativeEngine: initializing display.");
        mEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (EGL_FALSE == eglInitialize(mEglDisplay, 0, 0)) {
         //   ALOGE("NativeEngine: failed to init display, error %d", eglGetError());
            return false;
        }
        return true;
    }

    bool AndroidWindow::InitSurface() {
        // need a display

        if (mEglSurface != EGL_NO_SURFACE) {
            // nothing to do
            //ALOGI("NativeEngine: no need to init surface (already had one).");
           // return true;
        }


        EGLint numConfigs;

        const EGLint attribs[] = {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, // request OpenGL ES 2.0
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_DEPTH_SIZE, 16,
                EGL_NONE
        };

        // since this is a simple sample, we have a trivial selection process. We pick
        // the first EGLConfig that matches:
        eglChooseConfig(mEglDisplay, attribs, &mEglConfig, 1, &numConfigs);

        // create EGL surface
        mEglSurface = eglCreateWindowSurface(mEglDisplay, mEglConfig, mApp->window, NULL);
        if (mEglSurface == EGL_NO_SURFACE) {
           // ALOGE("Failed to create EGL surface, EGL error %d", eglGetError());
            return false;
        }

        //ALOGI("NativeEngine: successfully initialized surface.");
        return true;
    }

    bool AndroidWindow::InitContext() {
        /// need a display
        //MY_ASSERT(mEglDisplay != EGL_NO_DISPLAY);

        EGLint attribList[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE}; // OpenGL ES 2.0

        if (mEglContext != EGL_NO_CONTEXT) {
            // nothing to do
          //  ALOGI("NativeEngine: no need to init context (already had one).");
           // return true;
        }

      //  ALOGI("NativeEngine: initializing context.");

        // create EGL context
        mEglContext = eglCreateContext(mEglDisplay, mEglConfig, NULL, attribList);
        if (mEglContext == EGL_NO_CONTEXT) {
           // ALOGE("Failed to create EGL context, EGL error %d", eglGetError());
            return false;
        }

       // ALOGI("NativeEngine: successfully initialized context.");

        return true;
    }

    Window* Window::create(int w, int h) {

        return new AndroidWindow();
    }


    bool AndroidWindow::init() {

        if (!InitDisplay()) {
            // ALOGE("NativeEngine: failed to create display.");
            return false;
        }

        // create surface if needed
        if (!InitSurface()) {
            // ALOGE("NativeEngine: failed to create surface.");
            return false;
        }

        // create context if needed
        if (!InitContext()) {
            // ALOGE("NativeEngine: failed to create context.");
            return false;
        }


        // bind
        if (EGL_FALSE == eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext)) {
            //ALOGE("NativeEngine: eglMakeCurrent failed, EGL error %d", eglGetError());
            HandleEglError(eglGetError());
        }

        //ALOGI("NativeEngine: binding surface and context (display %p, surface %p, context %p)",
        //  mEglDisplay, mEglSurface, mEglContext);

        if (!mHasGLObjects) {
            // ALOGI("NativeEngine: creating OpenGL objects.");
            if (!InitGLObjects()) {
                // ALOGE("NativeEngine: unable to initialize OpenGL objects.");
                return false;
            }
        }

        return true;
    }


    bool AndroidWindow::HandleEglError(EGLint error) {
        switch (error) {
            case EGL_SUCCESS:
                // nothing to do
                return true;
            case EGL_CONTEXT_LOST:
                //ALOGW("NativeEngine: egl error: EGL_CONTEXT_LOST. Recreating context.");
                KillContext();
                return true;
            case EGL_BAD_CONTEXT:
                //ALOGW("NativeEngine: egl error: EGL_BAD_CONTEXT. Recreating context.");
                KillContext();
                return true;
            case EGL_BAD_DISPLAY:
               // ALOGW("NativeEngine: egl error: EGL_BAD_DISPLAY. Recreating display.");
                KillDisplay();
                return true;
            case EGL_BAD_SURFACE:
                //ALOGW("NativeEngine: egl error: EGL_BAD_SURFACE. Recreating display.");
                KillSurface();
                return true;
            default:
                //ALOGW("NativeEngine: unknown egl error: %d", error);
                return false;
        }
    }


    void AndroidWindow::KillGLObjects() {
        if (mHasGLObjects) {
            SceneManager *mgr = SceneManager::GetInstance();
            mgr->KillGraphics();
            mHasGLObjects = false;
        }
    }

    void AndroidWindow::KillSurface() {
        //ALOGI("NativeEngine: killing surface.");
        eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (mEglSurface != EGL_NO_SURFACE) {
            eglDestroySurface(mEglDisplay, mEglSurface);
            mEglSurface = EGL_NO_SURFACE;
        }
        //ALOGI("NativeEngine: Surface killed successfully.");
    }

    void AndroidWindow::KillContext() {
        //ALOGI("NativeEngine: killing context.");

        // since the context is going away, we have to kill the GL objects
        KillGLObjects();

        eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (mEglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(mEglDisplay, mEglContext);
            mEglContext = EGL_NO_CONTEXT;
        }
        //ALOGI("NativeEngine: Context killed successfully.");
    }

    void AndroidWindow::KillDisplay() {
        // causes context and surface to go away too, if they are there
        //ALOGI("NativeEngine: killing display.");
        KillContext();
        KillSurface();

        if (mEglDisplay != EGL_NO_DISPLAY) {
            //ALOGI("NativeEngine: terminating display now.");
            eglTerminate(mEglDisplay);
            mEglDisplay = EGL_NO_DISPLAY;
        }
        //ALOGI("NativeEngine: display killed successfully.");
    }


    bool AndroidWindow::InitGLObjects() {
        if (!mHasGLObjects) {
            SceneManager *mgr = SceneManager::GetInstance();
            mgr->StartGraphics();
            log_opengl_error(glGetError());
            mHasGLObjects = true;
        }
        return true;
    }



    void AndroidWindow::SetVSync(bool enabled) {

    }

    bool AndroidWindow::IsVSync() const {
        return false;
    }

    uint32_t AndroidWindow::GetWidth() const {
        return 0;
    }

    uint32_t AndroidWindow::GetHeight() const {
        return 0;
    }

    void AndroidWindow::OnUpdate() {

    }

    bool AndroidWindow::getDoesSurfaceExist()
    {
        return mEglSurface != EGL_NO_SURFACE;
    }

    bool AndroidWindow::getDoesContextExist()
    {
        return mEglContext != EGL_NO_CONTEXT;
    }

    bool AndroidWindow::getDoesDisplayExist()
    {
        return mEglDisplay != EGL_NO_DISPLAY;
    }

    void AndroidWindow::getSurfaceSize(int &w, int &h) {
        int width, height;
        eglQuerySurface(mEglDisplay, mEglSurface, EGL_WIDTH, &width);
        eglQuerySurface(mEglDisplay, mEglSurface, EGL_HEIGHT, &height);

        w = width;
        h = height;
    }

    bool AndroidWindow::swapBuffers() {
        if (EGL_FALSE == eglSwapBuffers(mEglDisplay, mEglSurface)) {
            // failed to swap buffers...
            //ALOG("NativeEngine: eglSwapBuffers failed, EGL error %d", eglGetError());
            HandleEglError(eglGetError());

            return false;
        }

        return true;
    }

    void AndroidWindow::log_opengl_error(GLenum err) {
        while ((err = glGetError()) != GL_NO_ERROR) {
            //ALOGE("*** OpenGL error: error %d", err)
        }
    }

    void AndroidWindow::setAndroidApp(android_app *app) {
        mApp = app;
    }

}