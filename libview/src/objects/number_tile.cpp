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

#include "number_tile.hpp"
#include "../text.hpp"
#include "../colors.hpp"
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>

namespace libview::objects
{

namespace
{
    Magnum::Color3 value_to_color(const int value)
    {
        using namespace Magnum::Math::Literals;

        switch(value)
        {
            case 0:  return 0xa05252_rgbf;
            case 1:  return 0xdd3b3b_rgbf;
            case 2:  return 0xef7429_rgbf;
            case 3:  return 0xe5bb13_rgbf;
            case 4:  return 0xa1c927_rgbf;
            case 5:  return 0x2bcd73_rgbf;
            case 6:  return 0x25d7d2_rgbf;
            case 7:  return 0x3f94de_rgbf;
            case 8:  return 0x4c52e0_rgbf;
            case 9:  return 0x904ce0_rgbf;
            case 10: return 0xd84ce0_rgbf;
            case 11: return 0xe998ed_rgbf;
            case 12: return 0x989898_rgbf;
            case 13: return 0x461b3f_rgbf;
            default: return 0x000000_rgbf;
        }
    }
}

number_tile::number_tile(Object2D& parent, features::drawable_group& drawables, const int value):
    tile(parent, drawables),
    square_color_(value_to_color(value)),
    square_
    (
        *this,
        drawable_children_,
        "/res/images/rounded_square.tga",
        sdf_image::style
        {
            .color = Magnum::Color4{square_color_, 0.0f},
            .outline_color = Magnum::Color4{colors::black, 0.0f},
            .outline_range = {0.5f, 0.3f}
        }
    ),
    label_
    (
        *this,
        drawable_children_,
        std::to_string(value).c_str(),
        static_label::style
        {
            .alignment = Magnum::Text::Alignment::MiddleCenter,
            .font_size = 1.2f,
            .color = Magnum::Color4{colors::white, 0},
            .outline_color = Magnum::Color4{colors::dark_gray, 0},
            .outline_range = {0.45f, 0.40f}
        }
    )
{
}

void number_tile::set_alpha(const float alpha)
{
    square_.set_color({square_color_, alpha});
    label_.set_color({colors::white, alpha});
    label_.set_outline_color({colors::dark_gray, alpha});
}

void number_tile::draw(const Magnum::Matrix3& /*transformation_matrix*/, SceneGraph::Camera2D& camera)
{
    camera.draw(drawable_children_);
}

} //namespace
