
#include "display.h"
#include "displayerror.h"

namespace pinta {

Display::Display(int width, int height, const char *title):
    width(width), height(height)
{
    init(title);
}

Display::~Display()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::swap()
{
    SDL_GL_SwapWindow(window);
}

void Display::init(const char *title)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw DisplayError(SDL_GetError());
    }

    //SDL_ShowCursor(SDL_DISABLE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
        );
    if (!window)
        throw DisplayError(SDL_GetError());

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context)
        throw DisplayError(SDL_GetError());
}

}
