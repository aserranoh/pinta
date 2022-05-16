
#include <stdio.h>

#ifdef USE_DRM
#include <drm_fourcc.h>
#endif

#include "pinta.h"
#include "globals.h"

#define PINTA_MAX_ERRSTR_LENGTH 1024

// Contains the last error message
char errstr[PINTA_MAX_ERRSTR_LENGTH];

// Function to swap the buffers, depends on the type of frontend
static int (*pinta_swap) (void);

// Function to free frontend resources
static void (*pinta_destroy) (void);

#ifdef USE_DRM

static int
pinta_bo_swap()
{
    pinta_egl_swap();
    pinta_gbm_lock_fb();
    if (pinta_drm_fb_get_from_bo(gbm.next_bo)) {
        return -1;
    }
    return 0;
}

static int
pinta_swap_drm()
{
    int res = pinta_bo_swap()
        || pinta_drm_flip();
    if (res) {
        return -1;
    }

    // Release last buffer to render on again
    pinta_gbm_release_buffer();

    return 0;
}

static void
pinta_destroy_drm()
{
}

#endif

#ifdef USE_X

static int
pinta_swap_x()
{
    pinta_egl_swap();
    return 0;
}

static void
pinta_destroy_x()
{
    pinta_x_destroy();
}

#endif

static int
pinta_init_drm(int w, int h, const char *mode, unsigned int vrefresh,
    uint32_t format, uint64_t modifier)
{
#if USE_DRM

    int res = pinta_drm_init(0, mode, vrefresh)
        || pinta_gbm_init(w, h, format, modifier, drm.fd, drm.mode)
        || pinta_egl_init(PINTA_EGL_DRM, (EGLNativeDisplayType)gbm.dev,
            gbm.format, (EGLNativeWindowType)gbm.surface, 0)
        || pinta_gl_init(gbm.width, gbm.height)
        || pinta_bo_swap()
        || pinta_drm_set_mode();
    if (res) {
        return -1;
    }
    gbm.bo = gbm.next_bo;
    pinta_swap = pinta_swap_drm;
    pinta_destroy = pinta_destroy_drm;
    return 0;

#else

    sprintf(errstr, "support for DRM frontend not built");
    return -1;

#endif

}

static int
pinta_init_x(int w, int h)
{
#ifdef USE_X

    int res = pinta_x_init(w, h)
        || pinta_egl_init(PINTA_EGL_X, (EGLNativeDisplayType)xinfo.xdisplay, 0,
            (EGLNativeWindowType)xinfo.win, 0)
        || pinta_gl_init(w, h);
    if (res) {
        return -1;
    }
    pinta_swap = pinta_swap_x;
    pinta_destroy = pinta_destroy_x;
    return 0;

#else

    sprintf(errstr, "support for X frontend not built");
    return -1;

#endif

}

int
pinta_draw(struct pinta_model *model)
{
    pinta_gl_draw(model);
    return pinta_swap();
}

const char *
pinta_error()
{
    return errstr;
}

int
pinta_init(enum pinta_frontend frontend, int w, int h)
{
    return pinta_init_ext(frontend, w, h, 0, 0, 0, 0);
}

int
pinta_init_ext(enum pinta_frontend frontend, int w, int h, const char *mode,
    unsigned int vrefresh, uint32_t format, uint64_t modifier)
{
    switch (frontend) {
        case PINTA_AUTO:
            // TODO: implement
            break;
        case PINTA_X:
            return pinta_init_x(w, h);
        case PINTA_DRM:
            return pinta_init_drm(w, h, mode, vrefresh, format, modifier);
        default:
            sprintf(errstr, "unknown frontend");
            return -1;
    }
}

int
pinta_quit()
{
    pinta_egl_destroy();
    pinta_destroy();
}

