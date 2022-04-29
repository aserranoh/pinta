
#include <stdint.h>
#include <stdio.h>
#include <gbm.h>
#include <drm_fourcc.h>

#include "globals.h"

struct pinta_gbm gbm;

static int
init_surface(uint64_t modifier)
{
    if (gbm_surface_create_with_modifiers) {
        gbm.surface = gbm_surface_create_with_modifiers(
            gbm.dev, gbm.width, gbm.height, gbm.format, &modifier, 1);
    }
    if (!gbm.surface) {
        if (modifier != DRM_FORMAT_MOD_LINEAR) {
            sprintf(
                errstr, "Modifiers requested but support isn't available");
            return -1;
        }
        gbm.surface = gbm_surface_create(gbm.dev, gbm.width, gbm.height,
            gbm.format, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    }
    if (!gbm.surface) {
        sprintf(errstr, "failed to create gbm surface");
        return -1;
    }
    return 0;
}

int
pinta_gbm_init(int w, int h, uint32_t format, uint64_t modifier, int drm_fd,
    drmModeModeInfo *drm_mode)
{
    if (!format) {
        format = DRM_FORMAT_XRGB8888;
    }
    if (!modifier) {
        modifier = DRM_FORMAT_MOD_LINEAR;
    }

    gbm.dev = gbm_create_device(drm_fd);
    gbm.format = format;
    gbm.surface = NULL;

    gbm.width = (w != 0) ? w : drm_mode->hdisplay;
    gbm.height = (h != 0) ? h : drm_mode->vdisplay;

    return init_surface(modifier);
}

void
pinta_gbm_lock_fb()
{
    gbm.next_bo = gbm_surface_lock_front_buffer(gbm.surface);
}

void
pinta_gbm_release_buffer()
{
    gbm_surface_release_buffer(gbm.surface, gbm.bo);
    gbm.bo = gbm.next_bo;
}

