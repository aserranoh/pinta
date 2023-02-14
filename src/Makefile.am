
lib_LTLIBRARIES = libpinta.la
libpinta_la_SOURCES = clock.cpp color.cpp display.cpp displayerror.cpp floatcolor.cpp matrix.cpp mesh.cpp meshfactory.cpp renderedmesh.cpp renderer.cpp renderererror.cpp scene.cpp transformations.cpp vertex.cpp
nobase_include_HEADERS = pinta/clock.h pinta/color.h pinta/display.h pinta/displayerror.h pinta/floatcolor.h pinta/matrix.h pinta/mesh.h pinta/meshfactory.h pinta/renderedmesh.h pinta/renderer.h pinta/renderererror.h pinta/scene.h pinta/transformations.h pinta/vertex.h
libpinta_la_CXXFLAGS = $(sdl2_CXXLAGS) $(glesv2_CXXLAGS) -Ipinta
libpinta_la_LIBADD = $(sdl2_LIBS) $(glesv2_LIBS)

