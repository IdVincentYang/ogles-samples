# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# add_definitions(-DNDEBUG)

set(_APP_NAME "TouchMove")

add_library(${_APP_NAME}
    SHARED
    src/main/cpp/engine.cpp
    src/main/cpp/touch-move-main.cpp
)

# Force export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set_target_properties(${_APP_NAME}
    PROPERTIES
    LINK_FLAGS
    "-u ANativeActivity_onCreate"
)

set_target_properties(${_APP_NAME}
    PROPERTIES
    CXX_VISIBILITY_PRESET
    hidden
    C_VISIBILITY_PRESET
    hidden
)

include(AndroidNdkModules)
android_ndk_import_module_native_app_glue()

find_library(_LIB_ANDROID android)
find_library(_LIB_LOG log)
find_library(_LIB_EGL EGL)
find_library(_LIB_GLES2 GLESv2)

target_link_libraries(${_APP_NAME}
    native_app_glue
    ${_LIB_ANDROID}
    ${_LIB_EGL}
    ${_LIB_GLES2}
    ${_LIB_LOG}
)