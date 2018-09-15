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

#include "rectangle.hpp"

namespace libview
{

rectangle::rectangle
(
    SDL_Renderer& renderer,
    const geometry::rect& area,
    const SDL_Color& color
):
    renderer_(renderer),
    area_(area),
    color_(color)
{
}

void rectangle::set_position(const geometry::point& position)
{
    area_.pos = position;
}

void rectangle::draw(const geometry::system& sys)
{
    SDL_SetRenderDrawColor(&renderer_, color_.r, color_.g, color_.b, color_.a);

    const auto r = SDL_Rect
    {
        static_cast<int>(sys.unit * area_.pos.x + sys.origin.x),
        static_cast<int>(sys.unit * area_.pos.y + sys.origin.y),
        static_cast<int>(sys.unit * area_.w),
        static_cast<int>(sys.unit * area_.h)
    };
    SDL_RenderFillRect(&renderer_, &r);
}

} //namespace view
