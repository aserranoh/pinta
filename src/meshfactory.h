#ifndef PINTA_MESHFACTORY_H
#define PINTA_MESHFACTORY_H

#include "color.h"
#include "mesh.h"

namespace pinta {

Mesh * rectangle(float x, float y, float w, float h, float cornerRadius = 0, const Color &color = Color(0, 0, 0), int segments = 16);
Mesh * circle(float x, float y, float radius, const Color &color = Color(0, 0, 0), int segments = 32);

}

#endif