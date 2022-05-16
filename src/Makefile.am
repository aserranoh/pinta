
lib_LTLIBRARIES = libpinta.la
libpinta_la_SOURCES = clock.c cutil.h egl.c gl.c globals.h model.c pinta.c list.c
libpinta_la_CFLAGS = $(glesv2_CFLAGS) $(egl_CFLAGS)
libpinta_la_LIBADD = $(glesv2_LIBS) $(egl_LIBS)
if USE_DRM
libpinta_la_SOURCES += drm.c gbm.c
libpinta_la_CFLAGS += -DUSE_DRM $(libdrm_CFLAGS) $(gbm_CFLAGS)
libpinta_la_LIBADD += $(libdrm_LIBS) $(gbm_LIBS)
endif
if USE_X
libpinta_la_SOURCES += x.c
libpinta_la_CFLAGS += -DUSE_X $(x11_CFLAGS)
libpinta_la_LIBADD += $(x11_LIBS)
endif

include_HEADERS = pinta.h
