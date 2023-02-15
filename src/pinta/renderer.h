#ifndef PINTA_RENDERER_H
#define PINTA_RENDERER_H

#include <GLES2/gl2.h>
#include <list>
#include <unordered_map>

#include "pinta/floatcolor.h"
#include "pinta/matrix.h"
#include "pinta/mesh.h"
#include "pinta/renderedmesh.h"
#include "pinta/vector2.h"

namespace pinta {

class Renderer {

public:

    Renderer(int viewportWidth, int viewportHeight);
    ~Renderer();

    void clear();
    void disableStencilTest();
    void draw(const std::list<const Mesh *> &meshes);
    void enableStencilTest(bool enable);
    void resetTransformations();
    void setBackgroundColor(const FloatColor &color);
    void translate(const Vector2 &position);
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
    void rebuildVertexBuffers(const std::list<const Mesh *> &meshes);

    GLuint shaderProgram;
    GLint modelviewUniform;
    Matrix projectionMatrix;
    Matrix transformationMatrix;
    std::unordered_map<const Mesh *, RenderedMesh> renderedMeshes;
    GLuint vertexBuffer;
    GLuint indexBuffer;
    bool updateStencilEnabled;
    bool stencilTestEnabled;

};

}

#endif