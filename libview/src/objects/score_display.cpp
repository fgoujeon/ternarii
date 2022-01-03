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

#include "score_display.hpp"
#include "../text.hpp"
#include "../colors.hpp"
#include <libutil/to_string.hpp>
#include <Magnum/Shaders/Vector.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/GlyphCache.h>

namespace libview::objects
{

score_display::score_display(object2d& parent, features::drawable_group& drawables):
    object2d{&parent},
    features::drawable{*this, &drawables},
    renderer_(text::get_font(), text::get_glyph_cache(), 1.0f, Magnum::Text::Alignment::TopRight)
{
    renderer_.reserve(40, Magnum::GL::BufferUsage::DynamicDraw, Magnum::GL::BufferUsage::StaticDraw);
    renderer_.render("0");
}

void score_display::set_score(const int value)
{
    renderer_.render(libutil::to_string(value));
}

void score_display::draw(const Magnum::Matrix3& transformation_matrix, camera& camera)
{
    const auto absolute_alpha = get_absolute_alpha();

    text::get_shader().bindVectorTexture(text::get_glyph_cache().texture());
    text::get_shader().setTransformationProjectionMatrix(camera.projectionMatrix() * transformation_matrix);
    text::get_shader().setColor(colors::white * absolute_alpha);
    text::get_shader().setSmoothness(0.035f / transformation_matrix.uniformScaling());
    text::get_shader().setOutlineColor(colors::dark_gray * absolute_alpha);
    text::get_shader().setOutlineRange(0.47, 0.40);
    text::get_shader().draw(renderer_.mesh());
}

} //namespace
