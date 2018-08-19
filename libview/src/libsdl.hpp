#ifndef LIBSDL_HPP
#define LIBSDL_HPP

#include <SDL2/SDL.h>

namespace libsdl
{

struct session
{
    session()
    {
        SDL_Init(SDL_INIT_VIDEO);
    }

    ~session()
    {
        SDL_Quit();
    }
};

struct window
{
    template<class... Args>
    window(Args&&... args):
        ptr(SDL_CreateWindow(args...))
    {
    }

    ~window()
    {
        SDL_DestroyWindow(ptr);
    }

    SDL_Window* ptr;
};

struct renderer
{
    template<class... Args>
    renderer(Args&&... args):
        ptr(SDL_CreateRenderer(args...))
    {
    }

    ~renderer()
    {
        SDL_DestroyRenderer(ptr);
    }

    SDL_Renderer* ptr;
};

} //namespace libsdl

#endif
