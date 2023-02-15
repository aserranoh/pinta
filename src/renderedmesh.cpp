#include "pinta/renderedmesh.h"

namespace pinta {

RenderedMesh::RenderedMesh():
    mesh(nullptr), vertexOffset(0), indexOffset(0)
{

}

RenderedMesh::RenderedMesh(const Mesh *mesh, int vertexOffset, int indexOffset):
    mesh(mesh), vertexOffset(vertexOffset), indexOffset(indexOffset)
{

}

RenderedMesh::RenderedMesh(const RenderedMesh &other):
    mesh(other.mesh), vertexOffset(other.vertexOffset), indexOffset(other.indexOffset)
{

}

RenderedMesh::~RenderedMesh()
{

}

}