
#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef USE_DRM
#include <xf86drmMode.h>
#endif

#ifdef USE_X
#include <X11/Xlib.h>
#endif

#include <EGL/egl.h>

#include "pinta.h"

//////////////////////////////////// TYPES ////////////////////////////////////

#ifdef USE_DRM

/// DRM ///

struct drm_fb {
    struct gbm_bo *bo;
    uint32_t fb_id;
};

// Information about the DRM device
struct pinta_drm {
    int fd;
    drmModeModeInfo *mode;
    uint32_t crtc_id;
    int crtc_index;
    uint32_t connector_id;
    struct drm_fb *fb;
    int waiting_for_flip;
};

/// GBM ///

// Information about the GBM device
struct pinta_gbm {
    struct gbm_device *dev;
    uint32_t format;
    struct gbm_surface *surface;
    int width, height;
    struct gbm_bo *bo;
    struct gbm_bo *next_bo;
};

#endif

/// X ///

#ifdef USE_X

// Information about the X device
struct pinta_x {
    Display *xdisplay;
    Window win;
};

#endif

/// EGL ///

enum pinta_egl_frontend {
	PINTA_EGL_DRM,
	PINTA_EGL_X
};

/////////////////////////////////// GLOBALS ///////////////////////////////////

#ifdef USE_DRM

/// DRM ///

extern struct pinta_drm drm;

/// GBM ///

extern struct pinta_gbm gbm;

#endif

#if USE_X

/// X ///

extern struct pinta_x xinfo;

#endif

/// ERROR ///

extern char errstr[];

////////////////////////////////// FUNCTIONS //////////////////////////////////

#ifdef USE_DRM

/// DRM ///

int
pinta_drm_fb_get_from_bo(struct gbm_bo *bo);

int
pinta_drm_flip();

int
pinta_drm_init(const char *device, const char *mode_str,
    unsigned int vrefresh);

int
pinta_drm_set_mode();

/// GBM ///

int
pinta_gbm_init(int w, int h, uint32_t format, uint64_t modifier, int drm_fd,
    drmModeModeInfo *drm_mode);

void
pinta_gbm_lock_fb();

void
pinta_gbm_release_buffer();

#endif

#ifdef USE_X

/// X ///

int
pinta_x_init(int w, int h);

#endif

/// EGL ///

int
pinta_egl_init(enum pinta_egl_frontend frontend, EGLNativeDisplayType device,
	uint32_t format, EGLNativeWindowType surface, int samples);

void
pinta_egl_swap();

/// GL ///

void
pinta_gl_draw(struct pinta_model *model);

int
pinta_gl_init(int w, int h);

#endif

