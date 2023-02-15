#ifndef PINTA_RENDEREDMESH_H
#define PINTA_RENDEREDMESH_H

#include <GLES2/gl2.h>

#include "pinta/mesh.h"
#include "pinta/vertex.h"

namespace pinta {

class RenderedMesh {

public:

    RenderedMesh();
    RenderedMesh(const Mesh *mesh, int vertexOffset, int indexOffset);
    RenderedMesh(const RenderedMesh &other);
    ~RenderedMesh();

    inline const void * getColorOffset() const {return (const void *)(vertexOffset * sizeof(Vertex) + sizeof(float) * 2);}
    inline const void * getIndexOffset() const {return (const void *)(indexOffset * sizeof(unsigned short));}
    inline const void * getPositionOffset() const {return (const void *)(vertexOffset * sizeof(Vertex));}
    GLsizei getStride() const {return sizeof(Vertex);}

private:

    const Mesh *mesh;
    int indexOffset;
    int vertexOffset;

};

}

#endif