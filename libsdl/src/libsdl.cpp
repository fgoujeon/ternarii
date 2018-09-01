/*
Copyright 2018 Florian Goujeon

This file is part of Ternarii.

Ternarii is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ternarii is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ternarii.  If not, see <https://www.gnu.org/licenses/>.
*/

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
