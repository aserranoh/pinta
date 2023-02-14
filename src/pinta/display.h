#ifndef PINTA_DISPLAY_H
#define PINTA_DISPLAY_H

#include <SDL2/SDL.h>

namespace pinta {

class Display {

public:

    Display(int width, int height, const char *title = "");
    ~Display();

    inline int getWidth() const {return width;}
    inline int getHeight() const {return height;}

    void swap();

private:

    void init(const char *title);

    int width;
    int height;
    SDL_Window *window;

};

}

#endif
