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

#include "label.hpp"
#include "../text.hpp"

namespace libview::objects
{

label::label
(
    Object2D& parent,
    features::drawable_group& drawables,
    const style& stl,
    const std::string_view& value
):
    Object2D{&parent},
    features::drawable{*this, &drawables},
    renderer_(text::get_font(), text::get_glyph_cache(), stl.font_size, stl.alignment),
    style_(stl)
{
    set_text(value);
}

void label::set_text(const std::string_view& value)
{
    if(value.empty())
    {
        visible_ = false;
        return;
    }

    renderer_.reserve(value.size(), Magnum::GL::BufferUsage::DynamicDraw, Magnum::GL::BufferUsage::StaticDraw);
    renderer_.render(value.data());
    visible_ = true;
}

void label::draw(const Magnum::Matrix3& transformation_matrix, Magnum::SceneGraph::Camera2D& camera)
{
    if(visible_)
    {
        const auto color_transformation_matrix = get_color_transformation_matrix();

        text::get_shader().bindVectorTexture(text::get_glyph_cache().texture());
        text::get_shader().setTransformationProjectionMatrix(camera.projectionMatrix() * transformation_matrix);
        text::get_shader().setColor(color_transformation_matrix * style_.color);
        text::get_shader().setSmoothness(0.035f / (transformation_matrix.uniformScaling() * style_.font_size));
        text::get_shader().setOutlineColor(color_transformation_matrix * style_.outline_color);
        text::get_shader().setOutlineRange(style_.outline_range[0], style_.outline_range[1]);

        renderer_.mesh().draw(text::get_shader());
    }
}

} //namespace