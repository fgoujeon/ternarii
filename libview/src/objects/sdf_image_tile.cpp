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

#include "sdf_image_tile.hpp"
#include "../text.hpp"
#include "../colors.hpp"
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>

namespace libview::objects
{

sdf_image_tile::sdf_image_tile(Object2D& parent, features::drawable_group& drawables, const std::filesystem::path& image_path):
    tile{parent, drawables},
    square_color_(colors::light_gray),
    square_(*this, drawable_children_, "/res/images/rounded_square.tga"),
    image_(*this, drawable_children_, image_path)
{
    square_.set_color(Magnum::Color4{square_color_, 0.0f});

    image_.set_color(Magnum::Color4{colors::black, 0});
    image_.set_outline_color(Magnum::Color4{colors::black, 0});
}

void sdf_image_tile::set_alpha(const float alpha)
{
    square_.set_color({square_color_, alpha});
    image_.set_color({colors::black, alpha});
    image_.set_outline_color({colors::black, alpha});
}

void sdf_image_tile::draw(const Magnum::Matrix3& /*transformation_matrix*/, SceneGraph::Camera2D& camera)
{
    camera.draw(drawable_children_);
}

} //namespace
