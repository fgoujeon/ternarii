/*
Copyright 2018 - 2021 Florian Goujeon

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

#include "sdf_image_tile.hpp"
#include "../text.hpp"
#include "../colors.hpp"
#include <libres.hpp>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>

namespace libview::objects
{

sdf_image_tile::sdf_image_tile(object2d& parent, features::drawable_group& drawables, const std::filesystem::path& image_path):
    object2d{&parent},
    square_
    (
        *this,
        drawables,
        rounded_rectangle::style
        {
            .color = colors::light_gray,
            .dimension = {1.0f, 1.0f},
            .outline_color = colors::dark_gray,
            .outline_thickness = 0.04f,
            .radius = 0.5f
        }
    ),
    image_
    (
        *this,
        drawables,
        image_path,
        sdf_image::style
        {
            .color = colors::black,
            .outline_color = colors::black,
            .outline_range = {0.5f, 0.5f}
        }
    )
{
}

} //namespace
