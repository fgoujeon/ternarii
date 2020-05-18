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
    const mouse_press_callback& cb
):
    Object2D{&parent},
    features::clickable{*this, &clickables},
    mouse_press_callback_(cb),
    background_rectangle_(*this, drawables, colors::light_gray),
    label_(*this, drawables, text, 0.4f, Magnum::Text::Alignment::MiddleCenter)
{
    background_rectangle_.scale({1.5f, 0.35f});
    label_.set_color(colors::dark_gray);
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
    return -1.5 <= x && x <= 1.5 && -0.35 <= y && y <= 0.35;
}

void label_button::mouse_press_event()
{
    mouse_press_callback_();
}

} //namespace
