#include "renderer.h"
#include "renderererror.h"
#include "renderedmesh.h"
#include "transformations.h"

namespace pinta {

const char *Renderer::VERTEX_SHADER_TEXT = R"(
    uniform mat4 u_modelview;
    attribute vec4 a_position;
    attribute vec4 a_color;
    varying vec4 v_color;
    void main()
    {
        v_color = a_color;
        gl_Position = u_modelview * a_position;
    }
)";

const char *Renderer::FRAGMENT_SHADER_TEXT = R"(
    //precision mediump float;
    varying vec4 v_color;
    void main()
    {
        gl_FragColor = v_color;
    }
)";

GLuint Renderer::POS_ATTRIBUTE = 0;
GLuint Renderer::COLOR_ATTRIBUTE = 1;

Renderer::Renderer(int viewportWidth, int viewportHeight):
    updateStencilEnabled(false), stencilTestEnabled(false)
{
    createShaderProgram();
    linkProgram();
    glUseProgram(shaderProgram);
    glViewport(0, 0, viewportWidth, viewportHeight);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearStencil(0);
    glStencilFunc(GL_EQUAL, 1, 1);
    modelviewUniform = glGetUniformLocation(shaderProgram, "u_modelview");
    projectionMatrix = ortho(-viewportWidth/2.0, viewportWidth/2.0, -viewportHeight/2.0, viewportHeight/2.0, -1.0, 1.0);
}

Renderer::~Renderer()
{
    destroyBuffers();
}

void Renderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::disableStencilTest()
{
    glDisable(GL_STENCIL_TEST);
}

void Renderer::draw(const std::list<Mesh *> &meshes)
{
    for (Mesh *mesh: meshes) {
        if (renderedMeshes.count(mesh) == 0) {
            rebuildVertexBuffers(meshes);
            break;
        }
    }

    for (Mesh *mesh: meshes) {
        Matrix modelviewMatrix = translate(mesh->getPositionX(), mesh->getPositionY(), 0.0);
        Matrix modelviewProjectionMatrix = modelviewMatrix * projectionMatrix;
        glUniformMatrix4fv(modelviewUniform, 1, GL_FALSE, modelviewProjectionMatrix.data());

        const RenderedMesh &renderedMesh = renderedMeshes[mesh];
        glVertexAttribPointer(POS_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, renderedMesh.getStride(), renderedMesh.getPositionOffset());
        glVertexAttribPointer(COLOR_ATTRIBUTE, 4, GL_UNSIGNED_BYTE, GL_TRUE, renderedMesh.getStride(), renderedMesh.getColorOffset());
        glDrawElements(mesh->getPrimitive(), mesh->getIndexCount(), GL_UNSIGNED_SHORT, renderedMesh.getIndexOffset());
    }
}

void Renderer::enableStencilTest(bool enable)
{
    if (enable) {
		glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_EQUAL, 1, 1);
	} else {
        glStencilFunc(GL_ALWAYS, 1, 1);
		if (!updateStencilEnabled) {
			glDisable(GL_STENCIL_TEST);
		}
	}
	stencilTestEnabled = enable;
}

void Renderer::setBackgroundColor(const FloatColor &color)
{
    glClearColor(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
}

void Renderer::updateColor(bool update)
{
    glColorMask(update, update, update, update);
}

void Renderer::updateStencil(bool update)
{
    if (update) {
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	} else {
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		if (!stencilTestEnabled) {
			glDisable(GL_STENCIL_TEST);
		}
	}
	updateStencilEnabled = update;
}

void Renderer::createShaderProgram()
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, VERTEX_SHADER_TEXT);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_TEXT);
    shaderProgram = glCreateProgram();
    if (!shaderProgram) {
        throw RendererError("error on glCreateProgram");
    }
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
}

void Renderer::destroyBuffers()
{
    if (vertexBuffer) {
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &indexBuffer);
    }
    vertexBuffer = 0;
    indexBuffer = 0;
}

GLuint Renderer::loadShader(GLenum shaderType, const char *shaderSource)
{
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        throw RendererError("error on glCreateShader");
    }
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        std::string whichShader((shaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment");
        std::string strInfoLog;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char *infoLog = new char[infoLen];
            glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
            strInfoLog = infoLog;
            delete[] infoLog;
        }
        glDeleteShader(shader);
        throw RendererError(std::string("error compiling ") + whichShader + std::string("\n") + strInfoLog);
    }
    return shader;
}

void Renderer::linkProgram()
{
    glBindAttribLocation(shaderProgram, POS_ATTRIBUTE, "a_position");
    glBindAttribLocation(shaderProgram, COLOR_ATTRIBUTE, "a_color");
    glEnableVertexAttribArray(POS_ATTRIBUTE);
    glEnableVertexAttribArray(COLOR_ATTRIBUTE);
    glLinkProgram(shaderProgram);

    GLint linked;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        std::string strInfoLog;

        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char *infoLog = new char[infoLen];
            glGetProgramInfoLog(shaderProgram, infoLen, nullptr, infoLog);
            strInfoLog = infoLog;
            delete[] infoLog;
        }
        glDeleteProgram(shaderProgram);
        throw RendererError(std::string("error linking program:\n") + strInfoLog);
    }
}

void Renderer::rebuildVertexBuffers(const std::list<Mesh *> &meshes)
{
    std::vector<Vertex> vertices;
    std::vector<GLushort> indices;

    renderedMeshes.clear();
    int vertexOffset = 0;
    int indexOffset = 0;
    for (Mesh *mesh: meshes) {
        vertices.insert(vertices.end(), mesh->getVertices().begin(), mesh->getVertices().end());
        indices.insert(indices.end(), mesh->getIndices().begin(), mesh->getIndices().end());
        renderedMeshes[mesh] = RenderedMesh(mesh, vertexOffset, indexOffset);
        vertexOffset = vertices.size();
        indexOffset = indices.size();
    }

    destroyBuffers();
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
}

}