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

#include "shine.hpp"
#include "rounded_rectangle.hpp"
#include "label.hpp"
#include "../common.hpp"
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>

namespace libview::objects
{

class number_tile: public Object2D, public features::animable
{
    public:
        number_tile
        (
            Object2D& parent,
            features::drawable_group& drawables,
            features::animable_group& animables,
            const int value
        );

        void advance(const libutil::time_point& now, float elapsed_s);

    private:
        std::unique_ptr<shine> pshine0_;
        std::unique_ptr<shine> pshine1_;
        rounded_rectangle square_;
        std::unique_ptr<rounded_rectangle> pglow_;
        float glow_cycle_ = reinterpret_cast<int>(this) / 1000.0f; //cheap random
        label label_;
};

} //namespace

#endif
