#ifndef LIBSDL_HPP
#define LIBSDL_HPP

#include <SDL2/SDL.h>
#include <memory>

namespace libsdl
{

namespace detail
{
    struct window_deleter
    {
        void operator()(SDL_Window* ptr) const
        {
            SDL_DestroyWindow(ptr);
        }
    };

    struct renderer_deleter
    {
        void operator()(SDL_Renderer* ptr) const
        {
            SDL_DestroyRenderer(ptr);
        }
    };

    struct surface_deleter
    {
        void operator()(SDL_Surface* ptr) const
        {
            SDL_FreeSurface(ptr);
        }
    };

    struct texture_deleter
    {
        void operator()(SDL_Texture* ptr) const
        {
            SDL_DestroyTexture(ptr);
        }
    };
}

using window_unique_ptr   = std::unique_ptr<SDL_Window,   detail::window_deleter>;
using renderer_unique_ptr = std::unique_ptr<SDL_Renderer, detail::renderer_deleter>;
using surface_unique_ptr  = std::unique_ptr<SDL_Surface,  detail::surface_deleter>;
using texture_unique_ptr  = std::unique_ptr<SDL_Texture,  detail::texture_deleter>;

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

} //namespace libsdl

#endif
