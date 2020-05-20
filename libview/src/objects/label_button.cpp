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

#include "label_button.hpp"
#include "../text.hpp"
#include "../colors.hpp"
#include <Magnum/Text/AbstractFont.h>

namespace libview::objects
{

label_button::label_button
(
    Object2D& parent,
    features::drawable_group& drawables,
    features::clickable_group& clickables,
    const char* const text,
    const style& stl,
    const callback_set& callbacks
):
    Object2D{&parent},
    features::clickable{*this, &clickables},
    style_(stl),
    callbacks_(callbacks),
    background_rectangle_(*this, drawables, "/res/images/rounded_rectangle.tga"),
    label_
    (
        *this,
        drawables,
        text,
        stl.label
    )
{
    background_rectangle_.set_color(colors::light_gray);
    background_rectangle_.set_outline_color(colors::dark_gray);
}

void label_button::set_enabled(const bool enabled)
{
    enabled_ = enabled;
}

bool label_button::is_inside(const Magnum::Vector2& model_space_position) const
{
    if(!enabled_)
    {
        return false;
    }

    const auto x = model_space_position.x();
    const auto y = model_space_position.y();
    return -1 <= x && x <= 1 && -0.2 <= y && y <= 0.2;
}

void label_button::handle_mouse_press()
{
    background_rectangle_.set_color(colors::white);
    callbacks_.mouse_press_callback();
}

void label_button::handle_mouse_release()
{
    background_rectangle_.set_color(colors::light_gray);
    callbacks_.mouse_release_callback();
}

void label_button::handle_mouse_enter()
{
    background_rectangle_.set_color(colors::white);
}

void label_button::handle_mouse_leave()
{
    background_rectangle_.set_color(colors::light_gray);
}

} //namespace
