#include "libsdl.hpp"

namespace libsdl
{

namespace
{
    libsdl::texture_unique_ptr make_texture_from_surface
    (
        SDL_Renderer& renderer,
        SDL_Surface& surface
    )
    {
        return libsdl::texture_unique_ptr
        {
            SDL_CreateTextureFromSurface(&renderer, &surface)
        };
    }
}

libsdl::texture_unique_ptr make_texture
(
    SDL_Renderer& renderer,
    TTF_Font& font,
    const std::string& text,
    const SDL_Color& color
)
{
    auto psurface = libsdl::surface_unique_ptr
    {
        TTF_RenderText_Blended
        (
            &font,
            text.c_str(),
            color
        )
    };
    return make_texture_from_surface(renderer, *psurface);
}

} //namespace libsdl
