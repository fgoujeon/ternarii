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
    template<class T>
    struct deleter;

    template<>
    struct deleter<SDL_Window>
    {
        void operator()(SDL_Window* ptr) const
        {
            SDL_DestroyWindow(ptr);
        }
    };

    template<>
    struct deleter<SDL_Renderer>
    {
        void operator()(SDL_Renderer* ptr) const
        {
            SDL_DestroyRenderer(ptr);
        }
    };

    template<>
    struct deleter<SDL_Surface>
    {
        void operator()(SDL_Surface* ptr) const
        {
            SDL_FreeSurface(ptr);
        }
    };

    template<>
    struct deleter<SDL_Texture>
    {
        void operator()(SDL_Texture* ptr) const
        {
            SDL_DestroyTexture(ptr);
        }
    };

    template<>
    struct deleter<TTF_Font>
    {
        void operator()(TTF_Font* ptr) const
        {
            TTF_CloseFont(ptr);
        }
    };
}

template<class T>
using unique_ptr = std::unique_ptr<T, detail::deleter<T>>;

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

unique_ptr<SDL_Texture> make_texture
(
    SDL_Renderer& renderer,
    TTF_Font& font,
    const std::string& text,
    const SDL_Color& color
);

} //namespace libsdl

#endif
