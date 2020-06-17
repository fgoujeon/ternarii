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

#ifndef LIBVIEW_OBJECTS_SHINE_HPP
#define LIBVIEW_OBJECTS_SHINE_HPP

#include "../common.hpp"
#include <libutil/time.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>

namespace libview::objects
{

class shine: public object2d, public features::drawable, public features::animable
{
    public:
        struct style
        {
            Magnum::Color4 color;
            int ray_count = 16;
            float ray_width = 0.5f; //normalized
            float speed_radps = 0.05f; //in radians per second
        };

    public:
        shine
        (
            object2d& parent,
            features::drawable_group& drawables,
            features::animable_group& animables,
            const style& stl
        );

    private:
        void draw(const Magnum::Matrix3& transformation_matrix, camera& camera) override;

        void advance(const libutil::time::point& now, float elapsed_s) override;

    private:
        style style_;
        float angle_rad_ = reinterpret_cast<int>(this) / 1000.0; //cheap random
};

} //namespace

#endif
