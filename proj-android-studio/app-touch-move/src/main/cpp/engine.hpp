#pragma once

#include <EGL/egl.h>
#include <GLES2/gl2.h>

class Engine {
public:
    void init(EGLNativeWindowType nativeWindow);
    void drawFrame();
    void onTouchEvent(float x, float y);

private:
    void _loadProgram();
    float mInit = false;
    float mX = 0;
    float mY = 0;

    EGLDisplay mEGLDisplay = 0;
    EGLContext mEGLContext = 0;
    EGLSurface mEGLSurface = 0;
    EGLint mDisplayWidth = 0;
    EGLint mDisplayHeight = 0;

    GLuint mProgram;
};
