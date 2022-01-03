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

#ifndef LIBVIEW_OBJECTS_ROUNDED_SQUARE_HPP
#define LIBVIEW_OBJECTS_ROUNDED_SQUARE_HPP

#include "../common.hpp"
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>

namespace libview::objects
{

class rounded_rectangle: public object2d, public features::drawable
{
    public:
        struct style
        {
            Magnum::Color4 color;
            Magnum::Vector2 dimension = {1.0f, 1.0f}; //normalized rectangle dimension relative to unit square
            Magnum::Color4 outline_color;
            float outline_thickness = 0;
            float radius = 0;
            float smoothness_factor = 1;
        };

    public:
        rounded_rectangle
        (
            object2d& parent,
            features::drawable_group& drawables,
            const style& stl
        );

        void set_color(const Magnum::Color4& color);

    private:
        void draw(const Magnum::Matrix3& transformation_matrix, camera& camera) override;

    private:
        style style_;
};

} //namespace

#endif
