
#include "mesh.h"

namespace pinta {

Mesh::Mesh(float x, float y, GLenum primitive):
    x(x), y(y), primitive(primitive)
{
}

void Mesh::setColor(const Color &color)
{
    for (Vertex &vertex: vertices) {
        vertex.setColor(color);
    }
}

void Mesh::setPosition(float x, float y)
{
    this->x = x;
    this->y = y;
}

}
