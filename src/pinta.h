
#ifndef PINTA_H
#define PINTA_H

#include <stdint.h>
#include <stdlib.h>

#include <GLES2/gl2.h>

//////////////////////////////////   TYPES   //////////////////////////////////

/* pinta_frontend
   Type of frontend to use.
 */
enum pinta_frontend {
    PINTA_AUTO,
    PINTA_X,
    PINTA_DRM
};

/* pinta_clock
   Defines a clock to synchronize the rendering.
 */
struct pinta_clock {
    // Time of last execution
    uint64_t time;

    // Interval to wait between rendering cycles
    uint64_t interval;
};

/* pinta_point
   A 2D point
 */
typedef float pinta_point[2];

/* pinta_vertex
   Represents a vertex of a mesh.
 */
struct pinta_vertex {
    pinta_point pos;
    unsigned char color[4];
};

/* pinta_mesh
   Represents a mesh with vertices and triangular faces.
 */
struct pinta_mesh {
    float x;
    float y;
    int voffset;
    size_t nvertices;
    int ioffset;
    size_t nindices;
    GLenum primitive;
    struct pinta_mesh *prev;
    struct pinta_mesh *next;
};

/* pinta_list_node
   A node in a list.
 */
struct pinta_list_node {
    struct pinta_list_node *prev;
    struct pinta_list_node *next;
    void *data;
};

/* pinta_list
   A list of things
 */
struct pinta_list {
    struct pinta_list_node *first;
    struct pinta_list_node *last;
};

/* pinta_model
   A set of meshes to draw.
 */
struct pinta_model {
    // Background color
    float color[3];
    int color_changed;

    // Meshes in the model, in a linked list
    struct pinta_list meshes;

    // Model vertices and indices
    size_t nvertices;
    size_t vcapacity;
    struct pinta_vertex *vertices;
    size_t nindices;
    size_t icapacity;
    unsigned short *indices;

    // OpenGL buffers (attribute buffer and index buffer)
    GLuint buffers[2];

    // Bit to mark if the model has to be rebuild
    int dirty;
};

/* pinta_shape
   A shape (polygon/polyline)
 */
struct pinta_shape {
    pinta_point *points;
    size_t npoints;
};

////////////////////////////////   FUNCTIONS   ////////////////////////////////

/* pinta_circle
   Add a circle mesh to the model.
   - model: the model where to add the circle.
   - x: circle's x position.
   - y: circle's y position.
   - radius: circle's radius.
   - color: circle's color.
   - segments: number of segments to use to draw the circle.
 */
struct pinta_mesh *
pinta_circle(struct pinta_model *model, float x, float y, float radius,
    unsigned char color[4], int segments);

/* pinta_clock_init
   Initializes a clock to synchronize the rendering time.
   - clk: instance of the clock.
   - fps: disared frames per second of the rendering.
 */
void
pinta_clock_init(struct pinta_clock *clk, double fps);

/* pinta_clock_tick
   Free the CPU until the next rendering.
   - clk: instance of the clock.
 */
void
pinta_clock_tick(struct pinta_clock *clk);

/* pinta_draw
   Draw the scene.
   - model: the model to draw
 */
int
pinta_draw(struct pinta_model *model);

/* pinta_error
   Returns a message of the last error encountered by the library.
 */
const char *
pinta_error();

/* pinta_init
   Initialize the display.
   - frontend: frontend to use (PINTA_X, PINTA_DRM or PINTA_AUTO)
   - w: width of the display area
   - h: height of the display area
 */
int
pinta_init(enum pinta_frontend frontend, int w, int h);

/* pinta_init_ext
   Initialize the display, but allows to pass more parameters.
   - frontend: frontend to use (PINTA_X, PINTA_DRM or PINTA_AUTO)
   - w: width of the display area
   - h: height of the display area
   - mode: TODO
   - vrefresh: TODO
   - format: TODO
   - modifier: TODO
 */
int
pinta_init_ext(enum pinta_frontend frontend, int w, int h, const char *mode,
    unsigned int vrefresh, uint32_t format, uint64_t modifier);

/* pinta_list_add
   Add an element to the given list
   - list: the list to add the element to
   - element: the element to add
 */
void
pinta_list_add(struct pinta_list *list, void *element);

/* pinta_list_destroy
   Destroy the given list
   - list: the list to destroy
 */
void
pinta_list_destroy(struct pinta_list *list);

/* pinta_list_foreach
   Macro to iterate over a pinta_list
 */
#define pinta_list_foreach(variable, node, list) \
    for ((node) = (list).first, (variable) = (node) ? (node)->data : 0; \
         node; \
         (node) = (node)->next, (variable) = (node) ? (node)->data : 0)

/* pinta_list_init
   Initialize the given list
   - list: the list to initialize
 */
void
pinta_list_init(struct pinta_list *list);

/* pinta_mesh_from_shapes
   Create a mesh from a outer shape and a list of shapes as holes
   - shape: the outer shape
   - holes: the list of holes
 */
struct pinta_mesh *
pinta_mesh_from_shapes(struct pinta_model *model, struct pinta_shape *shape,
    struct pinta_list *holes);

/* pinta_model_destroy
   Destroy the given model
   - model: the model to destroy.
 */
void
pinta_model_destroy(struct pinta_model *model);

/* pinta_model_init
   Initialize the given model
   - model: the model to initialize.
 */
void
pinta_model_init(struct pinta_model *model);

/* pinta_model_set_color
   Set the background color of the model.
   - model: the model for which the color has to be set.
   - bcolor: background color.
 */
void
pinta_model_set_color(struct pinta_model *model, float bcolor[3]);

/* pinta_quit
   Free the resources used by the library.
 */
int
pinta_quit();

/* pinta_rectangle
   Add a rectangle mesh to the model.
   - model: the model where to add the rectangle.
   - x: rectangle's x position.
   - y: rectangle's y position.
   - w: rectangle's width.
   - h: rectangle's height.
   - radius: corner radius.
   - color: rectangle's color.
   - segments: number of segments to use for the corners when using corner
               radius.
 */
struct pinta_mesh *
pinta_rectangle(struct pinta_model *model, float x, float y, float w, float h,
    float radius, unsigned char color[4], int segments);

/* pinta_shape_destroy
   Destroy the given shape
   - shape: the shape to destroy
 */
void
pinta_shape_destroy(struct pinta_shape *shape);

/* pinta_shape_rectangle
   Create a rectangular shape.
   - x: rectangle's x position.
   - y: rectangle's y position.
   - w: rectangle's width.
   - h: rectangle's height.
   - radius: corner radius.
   - segments: number of segments to use for the corners when using corner
               radius.
 */
struct pinta_shape *
pinta_shape_rectangle(float x, float y, float w, float h, float radius,
    int segments);

#endif

