
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <gbm.h>
#include <xf86drm.h>

#include "globals.h"

#define MAX_DRM_DEVICES 64

struct pinta_drm drm;

static int
get_resources(int fd, drmModeRes **resources)
{
    *resources = drmModeGetResources(fd);
    return (*resources == NULL) ? -1 : 0;
}

static int
find_drm_device(drmModeRes **resources)
{
    drmDevicePtr devices[MAX_DRM_DEVICES] = { NULL };
    int num_devices, fd = -1;

    num_devices = drmGetDevices2(0, devices, MAX_DRM_DEVICES);
    if (num_devices < 0) {
        sprintf(errstr, "drmGetDevices2 failed: %s", strerror(-num_devices));
        return -1;
    }

    for (int i = 0; i < num_devices; i++) {
        drmDevicePtr device = devices[i];
        int ret;

        if (!(device->available_nodes & (1 << DRM_NODE_PRIMARY))) {
            continue;
        }
        /* OK, it's a primary device. If we can get the
         * drmModeResources, it means it's also a
         * KMS-capable device.
         */
        fd = open(device->nodes[DRM_NODE_PRIMARY], O_RDWR);
        if (fd < 0) {
            continue;
        }
        ret = get_resources(fd, resources);
        if (!ret) {
            break;
        }
        close(fd);
        fd = -1;
    }
    drmFreeDevices(devices, num_devices);

    if (fd < 0) {
        sprintf(errstr, "no drm device found");
    }
    return fd;
}

static uint32_t
find_crtc_for_encoder(const drmModeRes *resources,
    const drmModeEncoder *encoder)
{
    int i;

    for (i = 0; i < resources->count_crtcs; i++) {
        // Possible_crtcs is a bitmask as described here:
        // https://dvdhrm.wordpress.com/2012/09/13/linux-drm-mode-setting-api
        const uint32_t crtc_mask = 1 << i;
        const uint32_t crtc_id = resources->crtcs[i];
        if (encoder->possible_crtcs & crtc_mask) {
            return crtc_id;
        }
    }

    // No match found
    return -1;
}

static uint32_t
find_crtc_for_connector(const drmModeRes *resources,
    const drmModeConnector *connector)
{
    int i;

    for (i = 0; i < connector->count_encoders; i++) {
        const uint32_t encoder_id = connector->encoders[i];
        drmModeEncoder *encoder = drmModeGetEncoder(drm.fd, encoder_id);
        if (encoder) {
            const uint32_t crtc_id = find_crtc_for_encoder(resources, encoder);
            drmModeFreeEncoder(encoder);
            if (crtc_id != 0) {
                return crtc_id;
            }
        }
    }

    // No match found
    return -1;
}

static void
page_flip_handler(int fd, unsigned int frame, unsigned int sec,
    unsigned int usec, void *data)
{
    // Suppress 'unused parameter' warnings
    (void)fd, (void)frame, (void)sec, (void)usec;

    int *waiting_for_flip = data;
    *waiting_for_flip = 0;
}

static void
drm_fb_destroy_callback(struct gbm_bo *bo, void *data)
{
    int drm_fd = gbm_device_get_fd(gbm_bo_get_device(bo));
    struct drm_fb *fb = data;

    if (fb->fb_id) {
        drmModeRmFB(drm_fd, fb->fb_id);
    }
    free(fb);
}

int
pinta_drm_fb_get_from_bo(struct gbm_bo *bo)
{
    int drm_fd = gbm_device_get_fd(gbm_bo_get_device(bo));
    struct drm_fb *fb = gbm_bo_get_user_data(bo);
    uint32_t width, height, format, strides[4] = {0}, handles[4] = {0},
         offsets[4] = {0}, flags = 0;
    int ret = -1;

    if (fb) {
        drm.fb = fb;
        return 0;
    }

    fb = calloc(1, sizeof *fb);
    fb->bo = bo;

    width = gbm_bo_get_width(bo);
    height = gbm_bo_get_height(bo);
    format = gbm_bo_get_format(bo);

    if (gbm_bo_get_handle_for_plane && gbm_bo_get_modifier &&
        gbm_bo_get_plane_count && gbm_bo_get_stride_for_plane &&
        gbm_bo_get_offset)
    {
        uint64_t modifiers[4] = {0};
        modifiers[0] = gbm_bo_get_modifier(bo);
        const int num_planes = gbm_bo_get_plane_count(bo);
        for (int i = 0; i < num_planes; i++) {
            handles[i] = gbm_bo_get_handle_for_plane(bo, i).u32;
            strides[i] = gbm_bo_get_stride_for_plane(bo, i);
            offsets[i] = gbm_bo_get_offset(bo, i);
            modifiers[i] = modifiers[0];
        }
        if (modifiers[0]) {
            flags = DRM_MODE_FB_MODIFIERS;
            fprintf(stderr, "using modifier %" PRIx64 "\n", modifiers[0]);
        }
        ret = drmModeAddFB2WithModifiers(drm_fd, width, height, format,
            handles, strides, offsets, modifiers, &fb->fb_id, flags);
    }

    if (ret) {
        if (flags) {
            fprintf(stderr, "modifiers failed\n");
        }
        memcpy(handles, (uint32_t [4]){gbm_bo_get_handle(bo).u32,0,0,0}, 16);
        memcpy(strides, (uint32_t [4]){gbm_bo_get_stride(bo),0,0,0}, 16);
        memset(offsets, 0, 16);
        ret = drmModeAddFB2(drm_fd, width, height, format, handles, strides,
            offsets, &fb->fb_id, 0);
    }

    if (ret) {
        sprintf(errstr, "failed to create fb: %s", strerror(errno));
        free(fb);
        return -1;
    }

    gbm_bo_set_user_data(bo, fb, drm_fb_destroy_callback);
    drm.fb = fb;
    return 0;
}

int
pinta_drm_flip()
{
    fd_set fds;
    drmEventContext evctx = {
        .version = 2,
        .page_flip_handler = page_flip_handler,
    };

    int ret = drmModePageFlip(drm.fd, drm.crtc_id, drm.fb->fb_id,
        DRM_MODE_PAGE_FLIP_EVENT, &(drm.waiting_for_flip));
    if (ret) {
        sprintf(errstr, "failed to queue page flip: %s", strerror(errno));
        return -1;
    }

    while (drm.waiting_for_flip) {
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(drm.fd, &fds);

        ret = select(drm.fd + 1, &fds, NULL, NULL, NULL);
        if (ret < 0) {
            sprintf(errstr, "select error: %s", strerror(errno));
            return -1;
        } else if (ret == 0) {
            sprintf(errstr, "select timeout");
            return -1;
        } else if (FD_ISSET(0, &fds)) {
            // TODO: really?
            sprintf(errstr, "user interrupted");
            return -1;
        }
        drmHandleEvent(drm.fd, &evctx);
    }
    drm.waiting_for_flip = 1;
    return 0;
}

int
pinta_drm_init(const char *device, const char *mode_str, unsigned int vrefresh)
{
    drmModeRes *resources;
    drmModeConnector *connector = NULL;
    drmModeEncoder *encoder = NULL;
    int i, ret, area;

    // Open the DRM device
    drm.fd = -1;
    if (device) {
        drm.fd = open(device, O_RDWR);
        ret = get_resources(drm.fd, &resources);
        if (ret < 0 && errno == EOPNOTSUPP) {
            sprintf(
                errstr, "%s does not look like a modeset device", device);
        }
    } else {
        drm.fd = find_drm_device(&resources);
    }
    if (drm.fd < 0) {
        return -1;
    }
    if (!resources) {
        sprintf(errstr, "drmModeGetResources failed: %s", strerror(errno));
        return -1;
    }

    // Find a connected connector
    for (i = 0; i < resources->count_connectors; i++) {
        connector = drmModeGetConnector(drm.fd, resources->connectors[i]);
        if (connector->connection == DRM_MODE_CONNECTED) {
            // It's connected, let's use this!
            break;
        }
        drmModeFreeConnector(connector);
        connector = NULL;
    }
    if (!connector) {
        sprintf(errstr, "no connected connector");
        return -1;
    }

    // Find user requested mode
    drm.mode = 0;
    if (mode_str && *mode_str) {
        for (i = 0; i < connector->count_modes; i++) {
            drmModeModeInfo *current_mode = &connector->modes[i];
            if (strcmp(current_mode->name, mode_str) == 0) {
                if (vrefresh == 0 || current_mode->vrefresh == vrefresh) {
                    drm.mode = current_mode;
                    break;
                }
            }
        }
        if (!drm.mode) {
            fprintf(stderr, "requested mode not found, using default mode\n");
        }
    }

    // Find preferred mode or the highest resolution mode
    if (!drm.mode) {
        for (i = 0, area = 0; i < connector->count_modes; i++) {
            drmModeModeInfo *current_mode = &connector->modes[i];
            if (current_mode->type & DRM_MODE_TYPE_PREFERRED) {
                drm.mode = current_mode;
                break;
            }
            int current_area = current_mode->hdisplay * current_mode->vdisplay;
            if (current_area > area) {
                drm.mode = current_mode;
                area = current_area;
            }
        }
    }
    if (!drm.mode) {
        sprintf(errstr, "could not find mode");
        return -1;
    }

    // Find encoder
    for (i = 0; i < resources->count_encoders; i++) {
        encoder = drmModeGetEncoder(drm.fd, resources->encoders[i]);
        if (encoder->encoder_id == connector->encoder_id) {
            break;
        }
        drmModeFreeEncoder(encoder);
        encoder = NULL;
    }
    if (encoder) {
        drm.crtc_id = encoder->crtc_id;
    } else {
        uint32_t crtc_id = find_crtc_for_connector(resources, connector);
        if (crtc_id == 0) {
            sprintf(errstr, "no crtc found");
            return -1;
        }
        drm.crtc_id = crtc_id;
    }
    for (i = 0; i < resources->count_crtcs; i++) {
        if (resources->crtcs[i] == drm.crtc_id) {
            drm.crtc_index = i;
            break;
        }
    }
    drmModeFreeResources(resources);
    drm.connector_id = connector->connector_id;
    drm.waiting_for_flip = 1;

    return 0;
}

int
pinta_drm_set_mode()
{
    int ret = drmModeSetCrtc(drm.fd, drm.crtc_id, drm.fb->fb_id, 0, 0,
        &drm.connector_id, 1, drm.mode);
    if (ret) {
        sprintf(errstr, "failed to set mode: %s", strerror(errno));
        return -1;
    }
    return 0;
}

