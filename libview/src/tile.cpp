/*
Copyright 2018 - 2019 Florian Goujeon

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

#include "tile.hpp"
#include "text.hpp"
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>

namespace libview
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

tile::tile(const int value, SceneGraph::DrawableGroup2D& drawables, Object2D* parent):
    Object2D{parent},
    SceneGraph::Drawable2D{*this, &drawables},
    square_color_(value_to_color(value)),
    square_(addChild<square>(Magnum::Color4{square_color_, 0.0f}, drawable_children_)),
    text_renderer_(text::get_font(), text::get_glyph_cache(), 1.2f, Magnum::Text::Alignment::MiddleCenter)
{
    text_renderer_.reserve(3, Magnum::GL::BufferUsage::DynamicDraw, Magnum::GL::BufferUsage::StaticDraw);
    text_renderer_.render(std::to_string(value));
}

void tile::set_alpha(const float alpha)
{
    square_.set_color({square_color_, alpha});
    alpha_ = alpha;
}

void tile::draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera)
{
    camera.draw(drawable_children_);

    text::get_shader().bindVectorTexture(text::get_glyph_cache().texture());
    text::get_shader().setColor(Magnum::Color4{0xffffff_rgbf, alpha_});
    text::get_shader().setTransformationProjectionMatrix(camera.projectionMatrix() * transformation_matrix);
    text_renderer_.mesh().draw(text::get_shader());
}

} //namespace
