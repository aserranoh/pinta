
#ifndef PINTA_H
#define PINTA_H

#include <stdint.h>

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

/* pinta_vertex
   Represents a vertex of a mesh.
 */
struct pinta_vertex {
    float pos[2];
    unsigned char color[4];
};

/* pinta_mesh
   Represents a mesh with vertices and triangular faces.
 */
struct pinta_mesh {
    float x;
    float y;
    int voffset;
    int nvertices;
    int ioffset;
    int nindices;
    GLenum primitive;
    struct pinta_mesh *prev;
    struct pinta_mesh *next;
};

/* pinta_model
   A set of meshes to draw.
 */
struct pinta_model {
    // Background color
    float color[3];
    int color_changed;

    // Meshes in the model, in a linked list
    struct pinta_mesh *first;
    struct pinta_mesh *last;

    // Model vertices and indices
    int nvertices;
    int vcapacity;
    struct pinta_vertex *vertices;
    int nindices;
    int icapacity;
    unsigned short *indices;

    // OpenGL buffers (attribute buffer and index buffer)
    GLuint buffers[2];

    // Bit to mark if the model has to be rebuild
    int dirty;
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

#endif

