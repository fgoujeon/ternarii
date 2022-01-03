/*
Copyright 2018 - 2022 Florian Goujeon

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

#ifndef LIBVIEW_OBJECTS_ADDER_TILE_HPP
#define LIBVIEW_OBJECTS_ADDER_TILE_HPP

#include "rounded_rectangle.hpp"
#include "label.hpp"
#include "sdf_image.hpp"
#include "../common.hpp"
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>

namespace libview::objects
{

class adder_tile: public object2d
{
    public:
        adder_tile
        (
            object2d& parent,
            features::drawable_group& drawables,
            const int value,
            const bool show_star = true
        );

    private:
        rounded_rectangle square_;
        std::unique_ptr<sdf_image> pstar_;
        label label_;
};

} //namespace

#endif
