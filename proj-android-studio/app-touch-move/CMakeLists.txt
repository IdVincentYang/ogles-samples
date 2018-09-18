# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(APP_NAME "TouchMove")

add_library(${APP_NAME}
    SHARED
    src/main/cpp/touch-move.cpp
)

# Import Library: native_app_glue
set(LIB_NATIVE_APP_GLUE_STATIC "libNativeAppGlue")
add_library(${LIB_NATIVE_APP_GLUE_STATIC}
    STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

find_library(LIB_ANDROID android)
find_library(LIB_LOG log)

# Force export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

target_link_libraries(${APP_NAME}
    ${LIB_NATIVE_APP_GLUE_STATIC}
    ${LIB_ANDROID}
    ${LIB_LOG}
)