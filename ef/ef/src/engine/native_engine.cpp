#include "common.hpp"
#include "src/demo_scene.hpp"
#include "scene_manager.hpp"
#include "native_engine.hpp"

#include "paddleboat/paddleboat.h"
#include <android/window.h>
#include "src/engine/platform/android/windowAndroid.hpp"

// verbose debug logs on?
#define VERBOSE_LOGGING 1

#if VERBOSE_LOGGING
#define VLOGD ALOGI
#else
#define VLOGD
#endif

// max # of GL errors to print before giving up
#define MAX_GL_ERRORS 200

static bool all_motion_filter(const GameActivityMotionEvent* event) {
    // Process all motion events
    return true;
}

static NativeEngine *_singleton = nullptr;

// workaround for internal bug b/149866792
static NativeEngineSavedState appState = {false};

NativeEngine::NativeEngine(struct android_app *app) {
    ALOGI("NativeEngine: initializing.");
    mApp = app;

    window = ef::Window::create(0,0);
    dynamic_cast<ef::AndroidWindow*>(window)->setAndroidApp(mApp);
    window->init();

    mHasFocus = mIsVisible = mHasWindow = false;


    mSurfWidth = mSurfHeight = 0;
    mApiVersion = 0;
    mScreenDensity = AConfiguration_getDensity(app->config);
    mActiveAxisIds = 0;
    mJniEnv = NULL;

    memset(&mState, 0, sizeof(mState));
    mIsFirstFrame = true;

    app->motionEventFilter = all_motion_filter;

    if (app->savedState != NULL) {
        // we are starting with previously saved state -- restore it
        mState = *(struct NativeEngineSavedState *) app->savedState;
    }

    // only one instance of NativeEngine may exist!
    MY_ASSERT(_singleton == NULL);
    _singleton = this;

    Paddleboat_init(GetJniEnv(), app->activity->javaGameActivity); //clazz);

    //ef::AndroidWindow *tmp = dynamic_cast<ef::AndroidWindow*>(window.get());
    //std::unique_ptr<ef::AndroidWindow> tempAndroidWindow;
    //if(tmp != nullptr)
    //{
    //    window.release();
    //    tempAndroidWindow.reset(tmp);
    //}


   // std::unique_ptr<ef::AndroidWindow> tempAndroidWindow = dynamic_cast<ef::AndroidWindow*>(window);


    VLOGD("NativeEngine: querying API level.");
    ALOGI("NativeEngine: API version %d.", mApiVersion);
    ALOGI("NativeEngine: Density %d", mScreenDensity);
}

NativeEngine *NativeEngine::GetInstance() {
    MY_ASSERT(_singleton != NULL);
    return _singleton;
}

NativeEngine::~NativeEngine() {
    VLOGD("NativeEngine: destructor running");
    Paddleboat_destroy(mJniEnv);

    if (mJniEnv) {
        ALOGI("Detaching current thread from JNI.");
        mApp->activity->vm->DetachCurrentThread();
        ALOGI("Current thread detached from JNI.");
        mJniEnv = NULL;
    }
    _singleton = NULL;
}

static void _handle_cmd_proxy(struct android_app *app, int32_t cmd) {
    NativeEngine *engine = (NativeEngine *) app->userData;
    engine->HandleCommand(cmd);
}

bool NativeEngine::IsAnimating() {
    return mHasFocus && mIsVisible && mHasWindow;
}

void NativeEngine::GameLoop() {
    mApp->userData = this;
    mApp->onAppCmd = _handle_cmd_proxy;
    //mApp->onInputEvent = _handle_input_proxy;

    auto activity = NativeEngine::GetInstance()->GetAndroidApp()->activity;
    GameActivity_setWindowFlags(activity,
                                AWINDOW_FLAG_KEEP_SCREEN_ON | AWINDOW_FLAG_TURN_SCREEN_ON |
                                AWINDOW_FLAG_FULLSCREEN |
                                AWINDOW_FLAG_SHOW_WHEN_LOCKED,
                                0);
    UpdateSystemBarOffset();

    while (1) {
        int events;
        struct android_poll_source *source;

        // If not animating, block until we get an event; if animating, don't block.
        while ((ALooper_pollAll(IsAnimating() ? 0 : -1, NULL, &events, (void **) &source)) >= 0) {

            // process event
            if (source != NULL) {
                source->process(mApp, source);
            }

            // are we exiting?
            if (mApp->destroyRequested) {
                return;
            }
        }

        HandleGameActivityInput();

        if (IsAnimating()) {
            DoFrame();
        }
    }
}

JNIEnv *NativeEngine::GetJniEnv() {
    if (!mJniEnv) {
        ALOGI("Attaching current thread to JNI.");
        if (0 != mApp->activity->vm->AttachCurrentThread(&mJniEnv, NULL)) {
            ALOGE("*** FATAL ERROR: Failed to attach thread to JNI.");
            ABORT_GAME;
        }
        MY_ASSERT(mJniEnv != NULL);
        ALOGI("Attached current thread to JNI, %p", mJniEnv);
    }

    return mJniEnv;
}

JNIEnv *NativeEngine::GetAppJniEnv() {
    if (!mAppJniEnv) {
        ALOGI("Attaching current thread to JNI.");
        if (0 != mApp->activity->vm->AttachCurrentThread(&mAppJniEnv, NULL)) {
            ALOGE("*** FATAL ERROR: Failed to attach thread to JNI.");
            ABORT_GAME;
        }
        MY_ASSERT(mAppJniEnv != NULL);
        ALOGI("Attached current thread to JNI, %p", mAppJniEnv);
    }

    return mAppJniEnv;
}

void NativeEngine::HandleCommand(int32_t cmd) {
    SceneManager *mgr = SceneManager::GetInstance();

    VLOGD("NativeEngine: handling command %d.", cmd);
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.
            VLOGD("NativeEngine: APP_CMD_SAVE_STATE");
            mState.mHasFocus = mHasFocus;
            mApp->savedState = malloc(sizeof(mState));
            *((NativeEngineSavedState *) mApp->savedState) = mState;
            mApp->savedStateSize = sizeof(mState);
            break;
        case APP_CMD_INIT_WINDOW:
            // We have a window!
            VLOGD("NativeEngine: APP_CMD_INIT_WINDOW");
            if (mApp->window != NULL) {
                mHasWindow = true;
                if (mApp->savedStateSize == sizeof(mState) && mApp->savedState != nullptr) {
                    mState = *((NativeEngineSavedState *) mApp->savedState);
                    mHasFocus = mState.mHasFocus;
                } else {
                    // Workaround APP_CMD_GAINED_FOCUS issue where the focus state is not
                    // passed down from NativeActivity when restarting Activity
                    mHasFocus = appState.mHasFocus;
                }
            }
            VLOGD("HandleCommand(%d): hasWindow = %d, hasFocus = %d", cmd,
                  mHasWindow ? 1 : 0, mHasFocus ? 1 : 0);
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is going away -- kill the surface
            VLOGD("NativeEngine: APP_CMD_TERM_WINDOW");
            window->KillSurface();
            mHasWindow = false;
            break;
        case APP_CMD_GAINED_FOCUS:
            VLOGD("NativeEngine: APP_CMD_GAINED_FOCUS");
            mHasFocus = true;
            mState.mHasFocus = appState.mHasFocus = mHasFocus;
            break;
        case APP_CMD_LOST_FOCUS:
            VLOGD("NativeEngine: APP_CMD_LOST_FOCUS");
            mHasFocus = false;
            mState.mHasFocus = appState.mHasFocus = mHasFocus;
            break;
        case APP_CMD_PAUSE:
            VLOGD("NativeEngine: APP_CMD_PAUSE");
            mgr->OnPause();
            break;
        case APP_CMD_RESUME:
            VLOGD("NativeEngine: APP_CMD_RESUME");
            mgr->OnResume();
            break;
        case APP_CMD_STOP:
            VLOGD("NativeEngine: APP_CMD_STOP");
            Paddleboat_onStop(mJniEnv);
            mIsVisible = false;
            break;
        case APP_CMD_START:
            VLOGD("NativeEngine: APP_CMD_START");
            Paddleboat_onStart(mJniEnv);
            mIsVisible = true;
            break;
        case APP_CMD_WINDOW_RESIZED:
        case APP_CMD_CONFIG_CHANGED:
            VLOGD("NativeEngine: %s", cmd == APP_CMD_WINDOW_RESIZED ?
                                      "APP_CMD_WINDOW_RESIZED" : "APP_CMD_CONFIG_CHANGED");
            // Window was resized or some other configuration changed.
            // Note: we don't handle this event because we check the surface dimensions
            // every frame, so that's how we know it was resized. If you are NOT doing that,
            // then you need to handle this event!
            break;
        case APP_CMD_LOW_MEMORY:
            VLOGD("NativeEngine: APP_CMD_LOW_MEMORY");
            // system told us we have low memory. So if we are not visible, let's
            // cooperate by deallocating all of our graphic resources.
            if (!mHasWindow) {
                VLOGD("NativeEngine: trimming memory footprint (deleting GL objects).");
                window->KillGLObjects();
            }
            break;
        case APP_CMD_WINDOW_INSETS_CHANGED:
            VLOGD("NativeEngine: APP_CMD_WINDOW_INSETS_CHANGED");
            UpdateSystemBarOffset();
            break;
        default:
            VLOGD("NativeEngine: (unknown command).");
            break;
    }

    //VLOGD("NativeEngine: STATUS: F%d, V%d, W%d, EGL: D %p, S %p, CTX %p, CFG %p",
    //      mHasFocus, mIsVisible, mHasWindow, window->mEglDisplay, window->mEglSurface, window->mEglContext, window->mEglConfig);
}

void NativeEngine::UpdateSystemBarOffset() {
    ARect insets;
    // Log all the insets types
    GameActivity_getWindowInsets(mApp->activity, GAMECOMMON_INSETS_TYPE_SYSTEM_BARS, &insets);
    mSystemBarOffset = insets.top;
}

bool NativeEngine::HandleInput(AInputEvent *event) {


    return false;
}

void NativeEngine::HandleGameActivityInput() {
    CheckForNewAxis();
    // Swap input buffers so we don't miss any events while processing inputBuffer.
    android_input_buffer* inputBuffer = android_app_swap_input_buffers(mApp);
    // Early exit if no events.
    if (inputBuffer == nullptr) return;

    if (inputBuffer->keyEventsCount != 0) {
        for (uint64_t i = 0; i < inputBuffer->keyEventsCount; ++i) {
            GameActivityKeyEvent* keyEvent = &inputBuffer->keyEvents[i];
            Paddleboat_processGameActivityKeyInputEvent(keyEvent, sizeof(GameActivityKeyEvent));
        }
        android_app_clear_key_events(inputBuffer);
    }
    if (inputBuffer->motionEventsCount != 0) {
        for (uint64_t i = 0; i < inputBuffer->motionEventsCount; ++i) {
            GameActivityMotionEvent* motionEvent = &inputBuffer->motionEvents[i];
            if (!Paddleboat_processGameActivityMotionInputEvent(motionEvent,
                                                               sizeof(GameActivityMotionEvent))) {
                // Didn't belong to a game controller, process it ourselves if it is a touch event
                //_cook_game_activity_motion_event(motionEvent, _cooked_event_callback);
            }
        }
        android_app_clear_motion_events(inputBuffer);
    }
}

void NativeEngine::CheckForNewAxis() {
    // Tell GameActivity about any new axis ids so it reports
    // their events
    const uint64_t activeAxisIds = Paddleboat_getActiveAxisMask();
    uint64_t newAxisIds = activeAxisIds ^mActiveAxisIds;
    if (newAxisIds != 0) {
        mActiveAxisIds = activeAxisIds;
        int32_t currentAxisId = 0;
        while (newAxisIds != 0) {
            if ((newAxisIds & 1) != 0) {
                ALOGI("Enable Axis: %d", currentAxisId);
                GameActivityPointerAxes_enableAxis(currentAxisId);
            }
            ++currentAxisId;
            newAxisIds >>= 1;
        }
    }
}


void NativeEngine::ConfigureOpenGL() {
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}


bool NativeEngine::PrepareToRender() {

    bool isWindowCreated;

    if (!window->getDoesSurfaceExist() || !window->getDoesDisplayExist() || !window->getDoesContextExist())
    {

        window = ef::Window::create(0,0);
        dynamic_cast<ef::AndroidWindow*>(window)->setAndroidApp(mApp);
        window->init();

         if(window != nullptr) isWindowCreated = true;
         else isWindowCreated = false;

        // configure our global OpenGL settings
        ConfigureOpenGL();

        return isWindowCreated;
    }

    // ready to render
    return false;
}

void NativeEngine::DoFrame() {
    // prepare to render (create context, surfaces, etc, if needed)
    if (!PrepareToRender()) {
        // not ready
        VLOGD("NativeEngine: preparation to render failed.");
        return;
    }

    SceneManager *mgr = SceneManager::GetInstance();

    // how big is the surface? We query every frame because it's cheap, and some
    // strange devices out there change the surface size without calling any callbacks...
    int width, height;
    window->getSurfaceSize(width, height);

    if (width != mSurfWidth || height != mSurfHeight) {
        // notify scene manager that the surface has changed size
        ALOGI("NativeEngine: surface changed size %dx%d --> %dx%d", mSurfWidth, mSurfHeight,
              width, height);
        mSurfWidth = width;
        mSurfHeight = height;
        mgr->SetScreenSize(mSurfWidth, mSurfHeight);
        glViewport(0, 0, mSurfWidth, mSurfHeight);
    }

    // if this is the first frame, install the demo scene
    if (mIsFirstFrame) {
        mIsFirstFrame = false;
        mgr->RequestNewScene(new DemoScene());
    }

    // render!
    mgr->DoFrame();

    // swap buffers
    window->swapBuffers();

    // print out GL errors, if any
    GLenum e;
    static int errorsPrinted = 0;
    while ((e = glGetError()) != GL_NO_ERROR) {
        if (errorsPrinted < MAX_GL_ERRORS) {
            window->log_opengl_error(e);
            ++errorsPrinted;
            if (errorsPrinted >= MAX_GL_ERRORS) {
                ALOGE("*** NativeEngine: TOO MANY OPENGL ERRORS. NO LONGER PRINTING.");
            }
        }
    }
}

android_app *NativeEngine::GetAndroidApp() {
    return mApp;
}

