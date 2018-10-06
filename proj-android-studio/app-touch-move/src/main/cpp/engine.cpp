#include "engine.hpp"

#include <cassert>
#include <memory>


#include <android/log.h>

#ifndef DNDEBUG
#define _LD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "ENGINE", __VA_ARGS__))
#define _LI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "ENGINE", __VA_ARGS__))
#define _LW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "ENGINE", __VA_ARGS__))
#define _LE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "ENGINE", __VA_ARGS__))
#else
#define _LD(...)
#define _LI(...)
#define _LW(...)
#define _LE(...)
#endif

const char SRC_VS[] = ""
        "attribute highp vec2  myVertex;"
        "void main(void) {"
        "    gl_Position = vec4(myVertex, 0, 1);"
        "    gl_PointSize = 90.0;"
        "}";

const char SRC_FS[] = ""
        "void main()"
        "{"
        "    gl_FragColor =  vec4(1.0,0.0,0.0,1.0);"
        "}";

#define ATTRIBUTE_VERTEX 0

static GLuint _compileShader(const GLenum type, const GLchar *source, const int32_t iSize) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, &iSize);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

void Engine::init(EGLNativeWindowType nativeWindow) {
    /**
    * Initialize an EGL context for the current display.
    */
    // initialize OpenGL ES and EGL

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attributes[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,  // Request OpenGL ES2.0
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint major, minor;
    eglInitialize(display, &major, &minor);
    _LI("eglInitialize version: %d.%d", major, minor);

    /* Here, the application chooses the configuration it desires.
     * find the best match if possible, otherwise use the very first one
     */
    eglChooseConfig(display, attributes, nullptr, 0, &numConfigs);
    assert(numConfigs);
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    config = supportedConfigs[0];

    eglChooseConfig(display, attributes, supportedConfigs.get(), numConfigs, &numConfigs);
    for (EGLint i = 0; i < numConfigs; i++) {
        auto &cfg = supportedConfigs[i];
        EGLint r, g, b, d;
        if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r) &&
            eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b) &&
            eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
            r == 8 && g == 8 && b == 8 && d == 0) {

            config = supportedConfigs[i];
            break;
        }
    }

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    surface = eglCreateWindowSurface(display, config, nativeWindow, NULL);
    EGLint contextAttrs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    context = eglCreateContext(display, config, NULL, contextAttrs);

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    mEGLDisplay = display;
    mEGLContext = context;
    mEGLSurface = surface;
    mDisplayWidth = w;
    mDisplayHeight = h;
    _LD("Display:(w: %d, h: %d", w, h);

    if (eglMakeCurrent(mEGLDisplay, mEGLSurface, mEGLSurface, mEGLContext) == EGL_FALSE) {
        _LW("Unable to eglMakeCurrent");
        return;
    }
    // Check openGL on the system
    GLenum gl_info[] = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
    for (GLenum name : gl_info) {
        const GLubyte *info = glGetString(name);
        _LI("OpenGL Info: %s", info);
    }

    _loadProgram();
    mInit = true;
}

void Engine::_loadProgram() {
    GLuint program;

    // Create shader program
    program = glCreateProgram();

    // Create and compile vertex shader
    GLuint vs = _compileShader(GL_VERTEX_SHADER, SRC_VS, sizeof(SRC_VS));
    assert(vs != 0);
    // Create and compile fragment shader
    GLuint fs = _compileShader(GL_FRAGMENT_SHADER, SRC_FS, sizeof(SRC_FS));
    assert(fs != 0);
    // Attach vertex shader to program
    glAttachShader(program, vs);

    // Attach fragment shader to program
    glAttachShader(program, fs);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    assert(status != 0);

    glBindAttribLocation(program, ATTRIBUTE_VERTEX, "myVertex");

    mProgram = program;
}

void Engine::onTouchEvent(float x, float y) {
    _LD("onTouchEvent(x: %.0f, y: %.0f)", x, y);
    mX = 2 * x / mDisplayWidth - 1;
    mY = 1 - 2 * y / mDisplayHeight;
}

void Engine::drawFrame() {
    if (mInit) {
        _LD("drawFrame");
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(mProgram);

        GLfloat positions[] = {
                mX, mY,
        };
        glEnableVertexAttribArray(ATTRIBUTE_VERTEX);
        glVertexAttribPointer(ATTRIBUTE_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, (const void *) positions);

        glDrawArrays(GL_POINTS, 0, 1);

        glDisableVertexAttribArray(ATTRIBUTE_VERTEX);

        eglSwapBuffers(mEGLDisplay, mEGLSurface);
    }
}