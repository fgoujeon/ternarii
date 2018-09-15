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

#ifndef LIBVIEW_DRAW_HPP
#define LIBVIEW_DRAW_HPP

#include "geometry.hpp"

namespace libview
{

inline
void draw_rect(SDL_Renderer& renderer, const geometry::system& s, const SDL_Rect& r)
{
    const auto r2 = SDL_Rect
    {
        static_cast<int>(s.unit * r.x + s.origin.x),
        static_cast<int>(s.unit * r.y + s.origin.y),
        static_cast<int>(s.unit * r.w),
        static_cast<int>(s.unit * r.h)
    };
    SDL_RenderFillRect(&renderer, &r2);
}

} //namespace libview

#endif
