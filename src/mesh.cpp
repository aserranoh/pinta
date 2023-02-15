
#include "pinta/mesh.h"

namespace pinta {

Mesh::Mesh(GLenum primitive):
    primitive(primitive)
{
}

void Mesh::setColor(const Color &color)
{
    for (Vertex &vertex: vertices) {
        vertex.setColor(color);
    }
}

}
