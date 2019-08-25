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

#include "static_label.hpp"
#include "text.hpp"

namespace libview
{

static_label::static_label
(
    const char* const value,
    const float font_size,
    const Magnum::Text::Alignment alignment,
    const Magnum::Color4& color,
    SceneGraph::DrawableGroup2D& drawables,
    Object2D* parent
):
    Object2D{parent},
    SceneGraph::Drawable2D{*this, &drawables},
    renderer_(text::get_font(), text::get_glyph_cache(), font_size, alignment),
    color_(color)
{
    renderer_.reserve(std::strlen(value), Magnum::GL::BufferUsage::DynamicDraw, Magnum::GL::BufferUsage::StaticDraw);
    renderer_.render(value);
}

void static_label::set_color(const Magnum::Color4& color)
{
    color_ = color;
}

void static_label::draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera)
{
    text::get_shader().bindVectorTexture(text::get_glyph_cache().texture());
    text::get_shader().setTransformationProjectionMatrix(camera.projectionMatrix() * transformation_matrix);
    text::get_shader().setColor(color_);
    text::get_shader().setSmoothness(0.035f / transformation_matrix.uniformScaling());
    renderer_.mesh().draw(text::get_shader());
}

} //namespace
