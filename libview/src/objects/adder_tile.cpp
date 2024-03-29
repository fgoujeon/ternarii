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

#include "adder_tile.hpp"
#include "../text.hpp"
#include "../colors.hpp"
#include <libres.hpp>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>

namespace libview::objects
{

namespace
{
    std::string get_label_text(const int value)
    {
        if(value >= 0)
        {
            return "+" + std::to_string(value);
        }

        return std::to_string(value);
    }
}

adder_tile::adder_tile
(
    object2d& parent,
    features::drawable_group& drawables,
    const int value,
    const bool show_star
):
    object2d(&parent),
    square_
    (
        *this,
        drawables,
        rounded_rectangle::style
        {
            .color = colors::light_gray,
            .outline_color = colors::dark_gray,
            .outline_thickness = 0.04f,
            .radius = 0.6f
        }
    ),
    label_
    (
        *this,
        drawables,
        label::style
        {
            .alignment = Magnum::Text::Alignment::MiddleCenter,
            .color = colors::black,
            .font_size = 1.0f
        },
        get_label_text(value).c_str()
    )
{
    if(show_star)
    {
        pstar_ = std::make_unique<sdf_image>
        (
            *this,
            drawables,
            libres::images::special_tile_modifier_star,
            sdf_image::style
            {
                .color = colors::black,
                .outline_color = colors::black,
                .outline_range = {0.5f, 0.5f}
            }
        );
    }
}

} //namespace
