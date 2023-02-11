#ifndef PINTA_MESH_H
#define PINTA_MESH_H

#include "vertex.h"

#include <GLES2/gl2.h>
#include <vector>

namespace pinta {

class Mesh {

public:

    Mesh(float x, float y, GLenum primitive);

    inline GLushort getIndexCount() const {return indices.size();}
    inline const std::vector<GLushort> & getIndices() const {return indices;}
    inline float getPositionX() const {return x;}
    inline float getPositionY() const {return y;}
    inline GLenum getPrimitive() const {return primitive;}
    inline const std::vector<Vertex> & getVertices() const {return vertices;}
    void setColor(const Color &color);
    inline void setIndices(const std::vector<GLushort> &indices) {this->indices = indices;}
    inline void setPrimitive(GLenum primitive) {this->primitive = primitive;}
    inline void setVertices(const std::vector<Vertex> &vertices) {this->vertices = vertices;}

private:

    float x;
    float y;
    GLenum primitive;
    std::vector<Vertex> vertices;
    std::vector<GLushort> indices;

};

}

#endif
