/*
Copyright 2018 - 2020 Florian Goujeon

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

#ifndef LIBVIEW_OBJECTS_TILE_HPP
#define LIBVIEW_OBJECTS_TILE_HPP

#include "../common.hpp"
#include <Magnum/Magnum.h>

namespace libview::objects
{

class tile: public Object2D
{
    public:
        tile(Object2D& parent):
            Object2D{&parent}
        {
        }

        virtual ~tile() = default;

        virtual float get_alpha() const = 0;

        virtual void set_alpha(const float alpha) = 0;
};

} //namespace

#endif
