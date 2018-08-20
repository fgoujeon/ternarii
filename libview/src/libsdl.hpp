#ifndef LIBSDL_HPP
#define LIBSDL_HPP

#include <SDL_ttf.h>
#include <SDL2/SDL.h>
#include <string>
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

    struct font_deleter
    {
        void operator()(TTF_Font* ptr) const
        {
            TTF_CloseFont(ptr);
        }
    };
}

using window_unique_ptr   = std::unique_ptr<SDL_Window,   detail::window_deleter>;
using renderer_unique_ptr = std::unique_ptr<SDL_Renderer, detail::renderer_deleter>;
using surface_unique_ptr  = std::unique_ptr<SDL_Surface,  detail::surface_deleter>;
using texture_unique_ptr  = std::unique_ptr<SDL_Texture,  detail::texture_deleter>;
using font_unique_ptr     = std::unique_ptr<TTF_Font,     detail::font_deleter>;

struct session
{
    session()
    {
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();
    }

    ~session()
    {
        SDL_Quit();
    }
};

libsdl::texture_unique_ptr make_texture
(
    SDL_Renderer& renderer,
    TTF_Font& font,
    const std::string& text,
    const SDL_Color& color
);

} //namespace libsdl

#endif
