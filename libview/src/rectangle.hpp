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

#ifndef LIBVIEW_RECTANGLE_HPP
#define LIBVIEW_RECTANGLE_HPP

#include "label.hpp"
#include "geometry.hpp"
#include <libsdl.hpp>

namespace libview
{

class rectangle
{
    public:
        rectangle
        (
            SDL_Renderer& renderer,
            const geometry::rect& area,
            const SDL_Color& color
        );

        void set_position(const geometry::point& position);

        void draw(const geometry::system& sys);

    private:
        void update_rect();

    private:
        SDL_Renderer& renderer_;
        geometry::rect area_;
        SDL_Color color_;

        geometry::system system_;
        bool must_update_rect_ = true;
        SDL_Rect rect_;
};

} //namespace libview

#endif
