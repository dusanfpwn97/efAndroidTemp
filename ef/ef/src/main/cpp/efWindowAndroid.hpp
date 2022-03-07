#pragma once

#include <efWindow.hpp>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "game-activity/native_app_glue/android_native_app_glue.h"


//#define LOG_TAG "EF Engine"

#include "Log.h"

#define ABORT_GAME { ALOGE("*** GAME ABORTING."); *((volatile char*)0) = 'a'; }
#define DEBUG_BLIP ALOGI("[ BLIP ]: %s:%d", __FILE__, __LINE__)

#define MY_ASSERT(cond) { if (!(cond)) { ALOGE("ASSERTION FAILED: %s", #cond); \
   ABORT_GAME; } }

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

#include "our_key_codes.hpp"

namespace ef
{
    class AndroidWindow : public efWindow
    {

        AndroidWindow();
        ~AndroidWindow() override;

        bool InitDisplay() override;
        bool InitSurface() override;
        bool InitContext() override;


        // EGL stuff
        EGLDisplay mEglDisplay;
        EGLSurface mEglSurface;
        EGLContext mEglContext;
        EGLConfig mEglConfig;


        // android_app structure
        struct android_app *mApp;
    };
}