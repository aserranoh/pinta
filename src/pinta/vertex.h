#ifndef PINTA_VERTEX_H
#define PINTA_VERTEX_H

#include "pinta/color.h"

namespace pinta {

class Vertex {

public:

    Vertex(float x = 0.0, float y = 0.0, const Color &color = {0, 0, 0});

    inline void setColor(const Color &color) {this->color = color;}

    float position[2];
    Color color;

};

}

#endif
