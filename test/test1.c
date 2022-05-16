
#include "pinta.h"

#include <stdio.h>
#include <stdlib.h>

struct pinta_clock clk;
struct pinta_model model;
struct pinta_mesh *rect;
struct pinta_mesh *circ;
struct pinta_mesh *hole;

void
test1_init()
{
    if (pinta_init(PINTA_X, 800, 480)) {
        fprintf(stderr, "error initializing pinta: %s\n", pinta_error());
        exit(1);
    }
    pinta_model_init(&model);
    float bcolor[] = {0.898, 0.508, 0.215};
    pinta_model_set_color(&model, bcolor);
    pinta_clock_init(&clk, 60);

    // Create a rectangle
    unsigned char rect_color[] = {255, 255, 255, 255};
    rect = pinta_rectangle(&model, 200, 50, 100, 100, 40.0, rect_color, 64);

    // Create a circle
    unsigned char circ_color[] = {0, 0, 0, 255};
    circ = pinta_circle(&model, 100, -30, 50, circ_color, 64);

    // Craete a rectangular shape
    struct pinta_shape *out_shape = pinta_shape_rectangle(
        -250, 50, 250, 250, 0, 0);

    // Create another
    struct pinta_shape *in_shape = pinta_shape_rectangle(
        -250, 50, 200, 100, 40, 64);

    // Create the mesh from the shapes
    /*struct pinta_list holes;
    pinta_list_init(&holes);
    pinta_list_add(&holes, in_shape);
    hole = pinta_mesh_from_shapes(&model, out_shape, &holes);
    pinta_list_destroy(&holes);
    pinta_shape_destroy(out_shape);
    pinta_shape_destroy(in_shape);*/
}

int
main(int argc, char **argv)
{
    test1_init();
    while (1) {
        if (pinta_draw(&model)) {
            fprintf(stderr, "error on render: %s\n", pinta_error());
            exit(1);
        }
        pinta_clock_tick(&clk);
        break;
    }
    pinta_model_destroy(&model);
    pinta_quit();
}

