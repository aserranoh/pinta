
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef GL_ES_VERSION_2_0
#include <GLES2/gl2.h>
#endif
#include <GLES2/gl2ext.h>

#include "globals.h"

static const char *vertex_shader_source =
        "uniform mat4 modelview;                   \n"
        "attribute vec4 a_position;                \n"
        "attribute vec4 a_color;                   \n"
        "varying vec4 v_color;                     \n"
        "void main()                               \n"
        "{                                         \n"
        "    v_color = a_color;                    \n"
        "    gl_Position = modelview * a_position; \n"
        "}                                         \n";

static const char *fragment_shader_source =
        "precision mediump float;    \n"
        "varying vec4 v_color;       \n"
        "void main()                 \n"
        "{                           \n"
        "    gl_FragColor = v_color; \n"
        "}                           \n";

static struct {
    GLuint program;
    GLint modelview_id;
    GLfloat projection[4][4];
} gl;

static GLuint
load_shader(const char *shader_src, GLenum type)
{
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);
    if (!shader) {
        sprintf(errstr, "cannot create shader");
        return 0;
    }
    // Load the shader source
    glShaderSource(shader, 1, &shader_src, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        GLint info_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 1) {
            char* info_log = malloc(sizeof(char) * info_len);
            glGetShaderInfoLog(shader, info_len, NULL, info_log);
            sprintf(errstr, "error compiling shader: %s", info_log);
            free(info_log);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static int
create_program(const char *vs_src, const char *fs_src)
{
    GLuint vertex_shader, fragment_shader, program;
    GLint ret;

    vertex_shader = load_shader(vs_src, GL_VERTEX_SHADER);
    fragment_shader = load_shader(fs_src, GL_FRAGMENT_SHADER);
    if (!vertex_shader || !fragment_shader) {
        return -1;
    }

    program = glCreateProgram();
    if (!program) {
        sprintf(errstr, "cannot create program");
        return -1;
    }

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    return program;
}

static int
link_program(unsigned program)
{
    GLint ret;

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &ret);
    if (!ret) {
        char *log;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &ret);
        if (ret > 1) {
            log = malloc(ret);
            glGetProgramInfoLog(program, ret, NULL, log);
            sprintf(errstr, "program linking failed: %s\n", log);
            free(log);
        } else {
            sprintf(errstr, "program linking failed\n");
        }
        glDeleteProgram(program);
        return -1;
    }
    return 0;
}

static void
pinta_gl_model_rebuild(struct pinta_model *model)
{
    // Free the previously used buffers
    if (model->buffers[0]) {
        glDeleteBuffers(2, model->buffers);
    }
    model->dirty = 0;
    model->buffers[0] = model->buffers[1] = 0;

    // If there's no vertices, exit
    if (!model->nvertices) {
        return;
    }

    // Generate and fill 2 new buffers
    glGenBuffers(2, model->buffers);
    glBindBuffer(GL_ARRAY_BUFFER, model->buffers[0]);
    glBufferData(GL_ARRAY_BUFFER,
        model->nvertices * sizeof(struct pinta_vertex), model->vertices,
        GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->nindices * sizeof(GLushort),
        model->indices, GL_STATIC_DRAW);
}

static void
pinta_gl_load_identity(GLfloat result[4][4])
{
    memset(result, 0x0, sizeof(GLfloat[4][4]));
    result[0][0] = 1.0f;
    result[1][1] = 1.0f;
    result[2][2] = 1.0f;
    result[3][3] = 1.0f;
}

static void
pinta_gl_matrix_multiply(GLfloat result[4][4], GLfloat srcA[4][4],
    GLfloat srcB[4][4])
{
    for (int i = 0; i < 4; i++) {
        result[i][0] = srcA[i][0] * srcB[0][0]
            + srcA[i][1] * srcB[1][0]
            + srcA[i][2] * srcB[2][0]
            + srcA[i][3] * srcB[3][0];
        result[i][1] = srcA[i][0] * srcB[0][1]
            + srcA[i][1] * srcB[1][1]
            + srcA[i][2] * srcB[2][1]
            + srcA[i][3] * srcB[3][1];
        result[i][2] = srcA[i][0] * srcB[0][2]
            + srcA[i][1] * srcB[1][2]
            + srcA[i][2] * srcB[2][2]
            + srcA[i][3] * srcB[3][2];
        result[i][3] = srcA[i][0] * srcB[0][3]
            + srcA[i][1] * srcB[1][3]
            + srcA[i][2] * srcB[2][3]
            + srcA[i][3] * srcB[3][3];
    }
}

static void
pinta_gl_ortho(GLfloat ortho[4][4], GLfloat left, GLfloat right,
    GLfloat bottom, GLfloat top, GLfloat nearZ, GLfloat farZ)
{
    GLfloat deltaX = right - left;
    GLfloat deltaY = top - bottom;
    GLfloat deltaZ = farZ - nearZ;

    if ((deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f))
        return;

    pinta_gl_load_identity(ortho);
    ortho[0][0] = 2.0f / deltaX;
    ortho[3][0] = -(right + left) / deltaX;
    ortho[1][1] = 2.0f / deltaY;
    ortho[3][1] = -(top + bottom) / deltaY;
    ortho[2][2] = -2.0f / deltaZ;
    ortho[3][2] = -(nearZ + farZ) / deltaZ;
}

static void
pinta_gl_translate(GLfloat result[4][4], GLfloat tx, GLfloat ty, GLfloat tz)
{
    pinta_gl_load_identity(result);
    result[3][0] = tx;
    result[3][1] = ty;
    result[3][2] = tz;
}

void
pinta_gl_draw(struct pinta_model *model)
{
    GLfloat modelview[4][4];
    GLfloat modelviewprojection[4][4];

    // Rebuild the model if it's dirty
    if (model->dirty) {
        pinta_gl_model_rebuild(model);
    }

    // Clear the background with the model's background color
    if (model->color_changed) {
        glClearColor(model->color[0], model->color[1], model->color[2], 1.0);
        model->color_changed = 0;
    }
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the model's elements
    struct pinta_mesh *current;
    for (current = model->first; current; current = current->next) {
        // Set the transformation matrix
        pinta_gl_translate(modelview, current->x, current->y, 0.0f);
        pinta_gl_matrix_multiply(
            modelviewprojection, modelview, gl.projection);
        glUniformMatrix4fv(
            gl.modelview_id, 1, GL_FALSE, &modelviewprojection[0][0]);

        // Draw the elements
        size_t aoffset = sizeof(struct pinta_vertex) * current->voffset;
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
            sizeof(struct pinta_vertex), (const void *)aoffset);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE,
            sizeof(struct pinta_vertex),
            (const void *)(aoffset + sizeof(GLfloat) * 2));
        glDrawElements(
            current->primitive, current->nindices, GL_UNSIGNED_SHORT,
            (const void *)(sizeof(unsigned short) * current->ioffset));
    }    
}

int
pinta_gl_init(int w, int h)
{
    int ret;

    ret = create_program(vertex_shader_source, fragment_shader_source);
    if (ret < 0) {
        return -1;
    }

    gl.program = ret;

    glBindAttribLocation(gl.program, 0, "a_position");
    glBindAttribLocation(gl.program, 1, "a_color");
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    ret = link_program(gl.program);
    if (ret) {
        return -1;
    }

    glUseProgram(gl.program);
    glViewport(0, 0, w, h);

    gl.modelview_id = glGetUniformLocation(gl.program, "modelview");

    // Prepare the modelview matrix
    pinta_gl_ortho(gl.projection, -w/2.0, w/2.0, -h/2.0, h/2.0, -1.0, 1.0);

    return 0;
}

