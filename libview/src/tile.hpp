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

#ifndef LIBVIEW_TILE_HPP
#define LIBVIEW_TILE_HPP

#include "point.hpp"
#include <libsdl.hpp>

namespace libview
{

class tile
{
    public:
        tile();

        const point& get_position() const;

        void set_position(const point& position);

        void set_size(const unsigned int w, const unsigned int h);

        void set_visible(const bool visible);

        void set_value(const unsigned int value);

        void draw(SDL_Renderer& renderer);

    private:
        libsdl::unique_ptr<TTF_Font> pfont_;
        point position_;
        unsigned int w_ = 0;
        unsigned int h_ = 0;
        bool visible_ = false;
        unsigned int value_ = 0;
};

} //namespace libview

#endif
