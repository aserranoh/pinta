#ifndef PINTA_RENDERER_H
#define PINTA_RENDERER_H

#include <GLES2/gl2.h>
#include <list>
#include <unordered_map>

#include "floatcolor.h"
#include "matrix.h"
#include "mesh.h"
#include "renderedmesh.h"

namespace pinta {

class Renderer {

public:

    Renderer(int viewportWidth, int viewportHeight);
    ~Renderer();

    void clear();
    void disableStencilTest();
    void draw(const std::list<Mesh *> &meshes);
    void enableStencilTest(bool enable);
    void setBackgroundColor(const FloatColor &color);
    void updateColor(bool update);
    void updateStencil(bool update);

private:

    static const char *VERTEX_SHADER_TEXT;
    static const char *FRAGMENT_SHADER_TEXT;
    static GLuint POS_ATTRIBUTE;
    static GLuint COLOR_ATTRIBUTE;

    void createShaderProgram();
    void destroyBuffers();
    GLuint loadShader(GLenum shaderType, const char *shaderSource);
    void linkProgram();
    void rebuildVertexBuffers(const std::list<Mesh *> &meshes);

    GLuint shaderProgram;
    GLint modelviewUniform;
    Matrix projectionMatrix;
    std::unordered_map<Mesh *, RenderedMesh> renderedMeshes;
    GLuint vertexBuffer;
    GLuint indexBuffer;
    bool updateStencilEnabled;
    bool stencilTestEnabled;

};

}

#endif