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

#ifndef LIBVIEW_OBJECTS_NUMBER_TILE_HPP
#define LIBVIEW_OBJECTS_NUMBER_TILE_HPP

#include "sdf_image.hpp"
#include "static_label.hpp"
#include "tile.hpp"
#include "../common.hpp"
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>

namespace libview::objects
{

class number_tile: public tile
{
    public:
        number_tile(Object2D& parent, features::drawable_group& drawables, const int value);

        float get_alpha() const override;

        void set_alpha(const float alpha) override;

    private:
        float alpha_ = 0;
        Magnum::Color3 square_color_;
        sdf_image square_;
        static_label label_;
};

} //namespace

#endif
