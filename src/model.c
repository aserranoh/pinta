
#include <math.h>

#include "pinta.h"
#include "cutil.h"

#define EPSILON 1

static struct pinta_mesh *
pinta_mesh_new(struct pinta_model *model, float x, float y, int nvertices,
    int nindices)
{
    // Allocate space in the vertices and indices arrays
    if (model->nvertices + nvertices > model->vcapacity) {
        model->vcapacity = model->vcapacity ? model->vcapacity * 2 : nvertices;
        new_copy(struct pinta_vertex, model->vcapacity, model->vertices);
    }
    if (model->nindices + nindices > model->icapacity) {
        model->icapacity = model->icapacity ? model->icapacity * 2 : nindices;
        new_copy(unsigned short, model->icapacity, model->indices);
    }

    // Allocate a new mesh
    struct pinta_mesh *mesh;
    new(struct pinta_mesh, 1, mesh);
    mesh->prev = mesh->next = NULL;

    // Put the new mesh on the linked list
    if (!model->first) {
        // Case where the list is empty
        model->first = model->last = mesh;
    } else {
        model->last->next = mesh;
        mesh->prev = model->last;
        model->last = mesh;
    }

    mesh->x = x;
    mesh->y = y;

    // Assign vertices and indices for the new mesh
    mesh->voffset = model->nvertices;
    model->nvertices += nvertices;
    mesh->nvertices = nvertices;
    mesh->ioffset = model->nindices;
    model->nindices += nindices;
    mesh->nindices = nindices;

    // Mark the model as dirty
    model->dirty = 1;

    return mesh;
}

static void
pinta_set_vertex(struct pinta_model *model, struct pinta_mesh *mesh, int index,
    float x, float y, unsigned char color[4])
{
    struct pinta_vertex *vertex = &(model->vertices[mesh->voffset + index]);
    vertex->pos[0] = x;
    vertex->pos[1] = y;
    for (int i = 0; i < 4; i++) {
        vertex->color[i] = color[i];
    }
}

static void
pinta_set_index(struct pinta_model *model, struct pinta_mesh *mesh, int index,
    unsigned short vindex)
{
    model->indices[mesh->ioffset + index] = vindex;
}

static void
pinta_segment(struct pinta_model *model, struct pinta_mesh *mesh, int segments,
    float start_angle, float angle, float x, float y, float radius,
    unsigned char color[4], int *next_vertex, int *next_index)
{
    // Compute the vertexs
    // Center vertex
    int center = *next_vertex;
    pinta_set_vertex(model, mesh, center, x, y, color);
    (*next_vertex)++;

    // Corner vertexs
    int i;
    for (i = 0; i <= segments; i++) {
        float phi = start_angle + angle/segments * i;
        pinta_set_vertex(model, mesh, *next_vertex, x + cos(phi)*radius,
            y + sin(phi)*radius, color);
        (*next_vertex)++;
    }

    // Faces
    for (i = 0; i < segments; i++) {
        pinta_set_index(model, mesh, *next_index, center);
        pinta_set_index(model, mesh, *next_index + 1, center + i + 2);
        pinta_set_index(model, mesh, *next_index + 2, center + i + 1);
        *next_index += 3;
    }
}

struct pinta_mesh *
pinta_circle(struct pinta_model *model, float x, float y, float radius,
    unsigned char color[4], int segments)
{
    // Allocate a new mesh on the model
    segments = (segments >= 3) ? segments : 3;
    struct pinta_mesh* mesh = pinta_mesh_new(
        model, x, y, segments + 1, segments + 2);

    // Vertices
    pinta_set_vertex(model, mesh, 0, 0, 0, color);
    int i;
    for (i = 0; i < segments; i++) {
        float phi = 2*M_PI/segments * i;
        pinta_set_vertex(
            model, mesh, i + 1, cos(phi)*radius, sin(phi)*radius, color);
    }

    // Indices
    pinta_set_index(model, mesh, 0, 0);
    pinta_set_index(model, mesh, 1, 1);
    int index = 2;
    for (i = segments; i >= 1; i--) {
        pinta_set_index(model, mesh, index, i);
        index++;
    }
    mesh->primitive = GL_TRIANGLE_FAN;
}

void
pinta_model_init(struct pinta_model *model)
{
    // Default color
    for (int i = 0; i < 3; i++) {
        model->color[i] = 0.0f;
    }
    model->color_changed = 1;

    // Initialize list of meshes
    model->first = model->last = NULL;

    // Initialize arrays of vertices and indices
    model->nvertices = model->vcapacity = 0;
    model->vertices = 0;
    model->nindices = model->icapacity = 0;
    model->indices = 0;

    // Initialize buffers
    model->buffers[0] = model->buffers[1] = 0;
    model->dirty = 0;
}

void
pinta_model_set_color(struct pinta_model *model, float bcolor[3])
{
    for (int i = 0; i < 3; i++) {
        model->color[i] = bcolor[i];
    }
    model->color_changed = 1;
}

struct pinta_mesh *
pinta_rectangle(struct pinta_model *model, float x, float y, float w, float h,
    float radius, unsigned char color[4], int segments)
{
    int h_collapsed = 0, w_collapsed = 0;
    int nvertices, nindices;

    // Compute the number of vertices and indices to use
    radius = fminf(fminf(w, h)/2.0, radius);
    if (fabsf(radius - w/2.0) < EPSILON) {
        w_collapsed = 1;
    }
    if (fabsf(radius - h/2.0) < EPSILON) {
        h_collapsed = 1;
    }
    if (radius <= 0) {
        nvertices = nindices = 4;
    } else {
        if (w_collapsed && h_collapsed) {
            return pinta_circle(model, x, y, radius, color, segments);
        }
        if (w_collapsed || h_collapsed) {
            nvertices = (segments/2 + 1) * 2 + 2;
            nindices = (segments/4) * 12 + 12;
        } else {
            nvertices = (segments/4 + 1) * 4 + 4;
            nindices = (segments/4) * 12 + 30;
        }
    }

    // Allocate a new mesh on the model
    struct pinta_mesh* mesh = pinta_mesh_new(model, x, y, nvertices, nindices);

    // Set vertices and indices
    if (radius <= 0) {
        pinta_set_vertex(model, mesh, 0, -w/2.0, -h/2.0, color);
        pinta_set_vertex(model, mesh, 1, -w/2.0, h/2.0, color);
        pinta_set_vertex(model, mesh, 2, w/2.0, -h/2.0, color);
        pinta_set_vertex(model, mesh, 3, w/2.0, h/2.0, color);
        pinta_set_index(model, mesh, 0, 0);
        pinta_set_index(model, mesh, 1, 1);
        pinta_set_index(model, mesh, 2, 2);
        pinta_set_index(model, mesh, 3, 3);
        mesh->primitive = GL_TRIANGLE_STRIP;
    } else {
        int next_vertex = 0, next_index = 0;
        int center1, center2, center3, center4;
        if (w_collapsed || h_collapsed) {
            segments = (segments/2 < 2) ? 2 : segments/2;
            if (w_collapsed) {
                center1 = next_vertex;
                pinta_segment(model, mesh, segments, 0.0, M_PI, 0.0,
                    h/2.0 - radius, radius, color, &next_vertex, &next_index);
                center2 = next_vertex;
                pinta_segment(model, mesh, segments, M_PI, M_PI, 0.0,
                    -h/2.0 + radius, radius, color, &next_vertex, &next_index);
            } else if (h_collapsed) {
                center1 = next_vertex;
                pinta_segment(model, mesh, segments, 3*M_PI/2.0, M_PI,
                    w/2.0 - radius, 0.0, radius, color, &next_vertex,
                    &next_index);
                center2 = next_vertex;
                pinta_segment(model, mesh, segments, M_PI/2.0, M_PI,
                    -w/2.0 + radius, 0.0, radius, color, &next_vertex,
                    &next_index);
            }
            pinta_set_index(model, mesh, next_index, center1);
            pinta_set_index(model, mesh, next_index + 1, center2 + 1);
            pinta_set_index(model, mesh, next_index + 2, center2 - 1);
            next_index += 3;
            pinta_set_index(model, mesh, next_index, center1);
            pinta_set_index(model, mesh, next_index + 1, center2);
            pinta_set_index(model, mesh, next_index + 2, center2 + 1);
            next_index += 3;
            pinta_set_index(model, mesh, next_index, center2);
            pinta_set_index(model, mesh, next_index + 1, center1);
            pinta_set_index(model, mesh, next_index + 2, center1 + 1);
            next_index += 3;
            pinta_set_index(model, mesh, next_index, center2);
            pinta_set_index(model, mesh, next_index + 1, center1 + 1);
            pinta_set_index(model, mesh, next_index + 2, next_vertex - 1);
            next_index += 3;
        } else {
            segments = (segments/4 < 1) ? 1 : segments/4;
            int center1 = next_vertex;
            pinta_segment(model, mesh, segments, 0.0, M_PI/2.0, w/2.0 - radius,
                h/2.0 - radius, radius, color, &next_vertex, &next_index);
            int center2 = next_vertex;
            pinta_segment(model, mesh, segments, M_PI/2.0, M_PI/2.0,
                -w/2.0 + radius, h/2.0 - radius, radius, color, &next_vertex,
                &next_index);
            int center3 = next_vertex;
            pinta_segment(model, mesh, segments, M_PI, M_PI/2.0,
                -w/2.0 + radius, -h/2.0 + radius, radius, color, &next_vertex,
                &next_index);
            int center4 = next_vertex;
            pinta_segment(model, mesh, segments, 3*M_PI/2.0, M_PI/2.0,
                w/2.0 - radius, -h/2.0 + radius, radius, color, &next_vertex,
                &next_index);

            pinta_set_index(model, mesh, next_index, center1);
            pinta_set_index(model, mesh, next_index + 1, center2 + 1);
            pinta_set_index(model, mesh, next_index + 2, center2 - 1);
            next_index += 3;
            pinta_set_index(model, mesh, next_index, center1);
            pinta_set_index(model, mesh, next_index + 1, center2);
            pinta_set_index(model, mesh, next_index + 2, center2 + 1);
            next_index += 3;
            pinta_set_index(model, mesh, next_index, center2);
            pinta_set_index(model, mesh, next_index + 1, center3 + 1);
            pinta_set_index(model, mesh, next_index + 2, center3 - 1);
            next_index += 3;
            pinta_set_index(model, mesh, next_index, center2);
            pinta_set_index(model, mesh, next_index + 1, center3);
            pinta_set_index(model, mesh, next_index + 2, center3 + 1);
            next_index += 3;
            pinta_set_index(model, mesh, next_index, center3);
            pinta_set_index(model, mesh, next_index + 1, center4 + 1);
            pinta_set_index(model, mesh, next_index + 2, center4 - 1);
            next_index += 3;
            pinta_set_index(model, mesh, next_index, center3);
            pinta_set_index(model, mesh, next_index + 1, center4);
            pinta_set_index(model, mesh, next_index + 2, center4 + 1);
            next_index += 3;
            pinta_set_index(model, mesh, next_index, center4);
            pinta_set_index(model, mesh, next_index + 1, center1 + 1);
            pinta_set_index(model, mesh, next_index + 2, next_vertex - 1);
            next_index += 3;
            pinta_set_index(model, mesh, next_index, center4);
            pinta_set_index(model, mesh, next_index + 1, center1);
            pinta_set_index(model, mesh, next_index + 2, center1 + 1);
            next_index += 3;
            pinta_set_index(model, mesh, next_index, center4);
            pinta_set_index(model, mesh, next_index + 1, center3);
            pinta_set_index(model, mesh, next_index + 2, center2);
            pinta_set_index(model, mesh, next_index + 3, center4);
            pinta_set_index(model, mesh, next_index + 4, center2);
            pinta_set_index(model, mesh, next_index + 5, center1);
        }
        mesh->primitive = GL_TRIANGLES;
    }

    return mesh;
}

