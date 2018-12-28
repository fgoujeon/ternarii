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

#ifndef LIBVIEW_2P_TILE_HPP
#define LIBVIEW_2P_TILE_HPP

#include "label.hpp"
#include "rectangle.hpp"
#include "geometry.hpp"
#include <libsdl.hpp>

namespace libview_2p
{

class tile
{
    public:
        tile
        (
            SDL_Renderer& renderer,
            const unsigned int value,
            const geometry::rect& area
        );

        const geometry::point& get_position() const;

        void set_position(const geometry::point& position);

        void set_visible(const bool visible);

        void draw(const geometry::system& sys);

    private:
        SDL_Renderer& renderer_;
        unsigned int value_ = 0;
        geometry::rect area_;
        SDL_Color background_color_;
        rectangle rectangle_;
        label number_label_;
        bool visible_ = false;
};

} //namespace libview_2p

#endif
