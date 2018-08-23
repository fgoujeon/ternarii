#include "libsdl.hpp"

namespace libsdl
{

namespace
{
    unique_ptr<SDL_Texture> make_texture_from_surface
    (
        SDL_Renderer& renderer,
        SDL_Surface& surface
    )
    {
        return unique_ptr<SDL_Texture>
        {
            SDL_CreateTextureFromSurface(&renderer, &surface)
        };
    }
}

unique_ptr<SDL_Texture> make_texture
(
    SDL_Renderer& renderer,
    TTF_Font& font,
    const std::string& text,
    const SDL_Color& color
)
{
    auto psurface = unique_ptr<SDL_Surface>
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
