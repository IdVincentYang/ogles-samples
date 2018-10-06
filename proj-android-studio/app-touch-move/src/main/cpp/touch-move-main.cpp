
#include <string>

#include <jni.h>

#include <android/log.h>
#include <android_native_app_glue.h>

#include "engine.hpp"

#ifndef DNDEBUG
#define _LD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "TOUCH-MOVE-MAIN", __VA_ARGS__))
#define _LI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "TOUCH-MOVE-MAIN", __VA_ARGS__))
#define _LW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "TOUCH-MOVE-MAIN", __VA_ARGS__))
#define _LE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "TOUCH-MOVE-MAIN", __VA_ARGS__))
#else
#define _LD(...)
#define _LI(...)
#define _LW(...)
#define _LE(...)
#endif

#define _MAIN_LOOP_TIMEOUT  -1

class AndroidMainDelegate {
public:
    AndroidMainDelegate(struct android_app *app, Engine *engine)
            : mAppState(app), mEngine(engine) {
    }

    static void handleCmd(struct android_app *app, int32_t cmd) {
        AndroidMainDelegate *delegate = (AndroidMainDelegate *) app->userData;
        switch (cmd) {
            case APP_CMD_INIT_WINDOW:
                delegate->_onCmdInitWindow();
                break;
            default:
                _LW("unhandled cmd: %d", cmd);
                break;
        }
    }

    static int32_t handleInput(android_app *app, AInputEvent *event) {
        AndroidMainDelegate *delegate = (AndroidMainDelegate *) app->userData;
        int32_t input_type = AInputEvent_getType(event);
        switch (input_type) {
            case AINPUT_EVENT_TYPE_MOTION:
                return delegate->_onInputMotionEvent(event);
            default:
                _LE("unhandled input event type: %d", input_type);
                return 0;
        }
    }

private:
    int _onCmdInitWindow() {
        _LD("_onCmdInitWindow");
        mEngine->init(mAppState->window);
        return 0;
    }

    int32_t _onInputMotionEvent(const AInputEvent *motion_event) {
//        int32_t action = AMotionEvent_getAction(motion_event);
//        unsigned int flags = action & AMOTION_EVENT_ACTION_MASK;
        float x = AMotionEvent_getX(motion_event, 0);
        float y = AMotionEvent_getY(motion_event, 0);
        mEngine->onTouchEvent(x, y);
        return 0;
    }

    struct android_app *mAppState = nullptr;
    Engine *mEngine = nullptr;
};

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
extern "C" void android_main(struct android_app *app_state) {

    //  Link state and delegate
    Engine engine;
    AndroidMainDelegate delegate(app_state, &engine);

    app_state->userData = &delegate;
    app_state->onAppCmd = AndroidMainDelegate::handleCmd;
    app_state->onInputEvent = AndroidMainDelegate::handleInput;

    while (app_state->destroyRequested == 0) {
        int events;
        android_poll_source *event_source;

        int ret_id = ALooper_pollAll(_MAIN_LOOP_TIMEOUT, nullptr, &events, (void **) &event_source);

        bool needDraw = false;
        if (ret_id >= 0) {
            if (event_source != nullptr) {
                _LI("process event source type id: %d", ret_id);
                if (event_source->process) {
                    event_source->process(app_state, event_source);
                }
                needDraw = true;
            }
        } else {
            switch (ret_id) {
                case ALOOPER_POLL_WAKE:
                    _LI("ALooper return ALOOPER_POLL_WAKE");
                    break;
                case ALOOPER_POLL_CALLBACK:
                    _LI("ALooper return ALOOPER_POLL_CALLBACK");
                    needDraw = true;
                    break;
                case ALOOPER_POLL_TIMEOUT:
                    _LI("ALooper return ALOOPER_POLL_TIMEOUT");
                    break;
                case ALOOPER_POLL_ERROR:
                    _LI("ALooper return ALOOPER_POLL_ERROR");
                    break;
                default:
                    _LE("ALooper return unhandled value: %d", ret_id);
                    break;
            }
        }

        if (needDraw) {
            engine.drawFrame();
        }
    }
}
