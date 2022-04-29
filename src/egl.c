
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef GL_ES_VERSION_2_0
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#include "globals.h"
#include <EGL/eglext.h>

#define get_proc(exts, ext, name) \
    do { \
        if (has_ext(exts, #ext)) { \
            egl.name = (void *)eglGetProcAddress(#name); \
        } \
    } while (0)

struct pinta_egl {
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;

    PFNEGLGETPLATFORMDISPLAYEXTPROC         eglGetPlatformDisplayEXT;
    PFNEGLCREATEIMAGEKHRPROC                eglCreateImageKHR;
    PFNEGLDESTROYIMAGEKHRPROC               eglDestroyImageKHR;
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC     glEGLImageTargetTexture2DOES;
    PFNEGLCREATESYNCKHRPROC                 eglCreateSyncKHR;
    PFNEGLDESTROYSYNCKHRPROC                eglDestroySyncKHR;
    PFNEGLWAITSYNCKHRPROC                   eglWaitSyncKHR;
    PFNEGLCLIENTWAITSYNCKHRPROC             eglClientWaitSyncKHR;
    PFNEGLDUPNATIVEFENCEFDANDROIDPROC       eglDupNativeFenceFDANDROID;

    /* AMD_performance_monitor */
    PFNGLGETPERFMONITORGROUPSAMDPROC        glGetPerfMonitorGroupsAMD;
    PFNGLGETPERFMONITORCOUNTERSAMDPROC      glGetPerfMonitorCountersAMD;
    PFNGLGETPERFMONITORGROUPSTRINGAMDPROC   glGetPerfMonitorGroupStringAMD;
    PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC glGetPerfMonitorCounterStringAMD;
    PFNGLGETPERFMONITORCOUNTERINFOAMDPROC   glGetPerfMonitorCounterInfoAMD;
    PFNGLGENPERFMONITORSAMDPROC             glGenPerfMonitorsAMD;
    PFNGLDELETEPERFMONITORSAMDPROC          glDeletePerfMonitorsAMD;
    PFNGLSELECTPERFMONITORCOUNTERSAMDPROC   glSelectPerfMonitorCountersAMD;
    PFNGLBEGINPERFMONITORAMDPROC            glBeginPerfMonitorAMD;
    PFNGLENDPERFMONITORAMDPROC              glEndPerfMonitorAMD;
    PFNGLGETPERFMONITORCOUNTERDATAAMDPROC   glGetPerfMonitorCounterDataAMD;

    bool modifiers_supported;
} egl;

static bool
has_ext(const char *extension_list, const char *ext)
{
    const char *ptr = extension_list;
    int len = strlen(ext);

    if (ptr == NULL || *ptr == '\0') {
        return false;
    }
    while (true) {
        ptr = strstr(ptr, ext);
        if (!ptr) {
            return false;
        }
        if (ptr[len] == ' ' || ptr[len] == '\0') {
            return true;
        }
        ptr += len;
    }
}

static int
match_config_to_visual(
    EGLDisplay egl_display, EGLint visual_id, EGLConfig *configs, int count)
{
    int i;

    for (i = 0; i < count; ++i) {
        EGLint id;
        if (!eglGetConfigAttrib(
            egl_display, configs[i], EGL_NATIVE_VISUAL_ID, &id))
        {
            continue;
        }
        if (id == visual_id) {
            return i;
        }
    }
    return -1;
}

static bool
egl_choose_config(EGLDisplay egl_display, const EGLint *attribs,
    EGLint visual_id, EGLConfig *config_out)
{
    EGLint count = 0;
    EGLint matched = 0;
    EGLConfig *configs;
    int config_index = -1;

    if (!eglGetConfigs(egl_display, NULL, 0, &count) || count < 1) {
        sprintf(errstr, "No EGL configs to choose from");
        return false;
    }
    configs = malloc(count * sizeof *configs);
    if (!configs) {
        sprintf(errstr, "out of memory error");
        return false;
    }
    if (!eglChooseConfig(egl_display, attribs, configs, count, &matched)
        || !matched)
    {
        sprintf(errstr, "No EGL configs with appropriate attributes");
        free(configs);
        return false;
    }
    if (!visual_id) {
        config_index = 0;
    }
    if (config_index == -1) {
        config_index = match_config_to_visual(
            egl_display, visual_id, configs, matched);
    }
    if (config_index != -1) {
        *config_out = configs[config_index];
    }
    free(configs);
    if (config_index == -1) {
        return false;
    }
    return true;
}

int
pinta_egl_init(enum pinta_egl_frontend frontend, EGLNativeDisplayType device,
    uint32_t format, EGLNativeWindowType surface, int samples)
{
    EGLint major, minor;

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_ALPHA_SIZE, 0,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SAMPLES, samples,
        EGL_NONE
    };
    const char *egl_exts_client, *egl_exts_dpy, *gl_exts;

    egl_exts_client = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    get_proc(egl_exts_client, EGL_EXT_platform_base, eglGetPlatformDisplayEXT);

    switch (frontend) {
        case PINTA_EGL_DRM:
            if (egl.eglGetPlatformDisplayEXT) {
                egl.display = egl.eglGetPlatformDisplayEXT(
                    EGL_PLATFORM_GBM_KHR, device, NULL);
            } else {
                egl.display = eglGetDisplay(device);
            }
            break;
        case PINTA_EGL_X:
            egl.display = eglGetDisplay(device);
            break;
        default:
            sprintf(errstr, "unknown EGL frontend");
            return -1;
    }
    if (egl.display == EGL_NO_DISPLAY) {
        sprintf(errstr, "unable to open connection to local windowing system");
        return -1;
    }

    if (!eglInitialize(egl.display, &major, &minor)) {
        sprintf(errstr, "failed to initialize EGL");
        return -1;
    }

    egl_exts_dpy = eglQueryString(egl.display, EGL_EXTENSIONS);
    get_proc(egl_exts_dpy, EGL_KHR_image_base, eglCreateImageKHR);
    get_proc(egl_exts_dpy, EGL_KHR_image_base, eglDestroyImageKHR);
    get_proc(egl_exts_dpy, EGL_KHR_fence_sync, eglCreateSyncKHR);
    get_proc(egl_exts_dpy, EGL_KHR_fence_sync, eglDestroySyncKHR);
    get_proc(egl_exts_dpy, EGL_KHR_fence_sync, eglWaitSyncKHR);
    get_proc(egl_exts_dpy, EGL_KHR_fence_sync, eglClientWaitSyncKHR);
    get_proc(egl_exts_dpy, EGL_ANDROID_native_fence_sync,
        eglDupNativeFenceFDANDROID);

    egl.modifiers_supported = has_ext(
        egl_exts_dpy, "EGL_EXT_image_dma_buf_import_modifiers");

    fprintf(stderr, "Using display %p with EGL version %d.%d\n",
        egl.display, major, minor);

    fprintf(stderr, "===================================\n");
    fprintf(stderr, "EGL information:\n");
    fprintf(stderr, "  version: \"%s\"\n",
        eglQueryString(egl.display, EGL_VERSION));
    fprintf(stderr, "  vendor: \"%s\"\n",
        eglQueryString(egl.display, EGL_VENDOR));
    fprintf(stderr, "  client extensions: \"%s\"\n", egl_exts_client);
    fprintf(stderr, "  display extensions: \"%s\"\n", egl_exts_dpy);
    fprintf(stderr, "===================================\n");

    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        sprintf(errstr, "failed to bind api EGL_OPENGL_ES_API");
        return -1;
    }

    if (!egl_choose_config(
        egl.display, config_attribs, format, &egl.config))
    {
        return -1;
    }

    egl.context = eglCreateContext(
        egl.display, egl.config, EGL_NO_CONTEXT, context_attribs);
    if (egl.context == NULL) {
        sprintf(errstr, "failed to create context");
        return -1;
    }

    egl.surface = eglCreateWindowSurface(
        egl.display, egl.config, surface, NULL);
    if (egl.surface == EGL_NO_SURFACE) {
        sprintf(errstr, "failed to create EGL surface");
        return -1;
    }

    // Connect the context to the surface
    eglMakeCurrent(egl.display, egl.surface, egl.surface, egl.context);

    gl_exts = (char *)glGetString(GL_EXTENSIONS);
    fprintf(stderr, "OpenGL ES 2.x information:\n");
    fprintf(stderr, "  version: \"%s\"\n", glGetString(GL_VERSION));
    fprintf(stderr, "  shading language version: \"%s\"\n",
        glGetString(GL_SHADING_LANGUAGE_VERSION));
    fprintf(stderr, "  vendor: \"%s\"\n", glGetString(GL_VENDOR));
    fprintf(stderr, "  renderer: \"%s\"\n", glGetString(GL_RENDERER));
    fprintf(stderr, "  extensions: \"%s\"\n", gl_exts);
    fprintf(stderr, "===================================\n");

    get_proc(gl_exts, GL_OES_EGL_image, glEGLImageTargetTexture2DOES);

    get_proc(gl_exts, GL_AMD_performance_monitor, glGetPerfMonitorGroupsAMD);
    get_proc(gl_exts, GL_AMD_performance_monitor, glGetPerfMonitorCountersAMD);
    get_proc(gl_exts, GL_AMD_performance_monitor,
        glGetPerfMonitorGroupStringAMD);
    get_proc(gl_exts, GL_AMD_performance_monitor,
        glGetPerfMonitorCounterStringAMD);
    get_proc(gl_exts, GL_AMD_performance_monitor,
        glGetPerfMonitorCounterInfoAMD);
    get_proc(gl_exts, GL_AMD_performance_monitor, glGenPerfMonitorsAMD);
    get_proc(gl_exts, GL_AMD_performance_monitor, glDeletePerfMonitorsAMD);
    get_proc(gl_exts, GL_AMD_performance_monitor,
        glSelectPerfMonitorCountersAMD);
    get_proc(gl_exts, GL_AMD_performance_monitor, glBeginPerfMonitorAMD);
    get_proc(gl_exts, GL_AMD_performance_monitor, glEndPerfMonitorAMD);
    get_proc(gl_exts, GL_AMD_performance_monitor,
        glGetPerfMonitorCounterDataAMD);

    return 0;
}

void
pinta_egl_swap()
{
    eglSwapBuffers(egl.display, egl.surface);
}

