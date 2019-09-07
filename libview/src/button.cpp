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

#include "button.hpp"
#include "text.hpp"
#include "colors.hpp"
#include <MagnumPlugins/FreeTypeFont/FreeTypeFont.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/Vector.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/GlyphCache.h>

namespace libview
{

button::button
(
    const std::filesystem::path& image_path,
    const mouse_press_callback& cb,
    SceneGraph::DrawableGroup2D& drawables,
    clickable_group& clickables,
    Object2D* parent
):
    Object2D{parent},
    clickable{*this, &clickables},
    mouse_press_callback_(cb),
    image_(addChild<sdf_image>(image_path, drawables))
{
    image_.set_color(colors::light_gray);
    image_.set_outline_color(colors::dark_gray);
}

bool button::is_inside(const Magnum::Vector2& model_space_position) const
{
    const auto x = model_space_position.x();
    const auto y = model_space_position.y();
    const auto squared_distance = x * x + y * y;
    return squared_distance <= 1;
}

void button::mouse_press_event()
{
    mouse_press_callback_();
}

} //namespace
