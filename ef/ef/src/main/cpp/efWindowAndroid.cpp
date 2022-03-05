//
// Created by dusan on 05-Mar-22.
//

#include "efWindowAndroid.hpp"
#include <android/window.h>

namespace ef
{


    AndroidWindow::AndroidWindow() {

    }

    AndroidWindow::~AndroidWindow() {

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
            return true;
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
            return true;
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

}