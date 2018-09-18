#include <jni.h>
#include <string>

#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "VK-SAMPLE", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "VK-SAMPLE", __VA_ARGS__))

extern "C" void android_main(struct android_app *app) {
    LOGE("native activity init");
}