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

#include "blank_button.hpp"
#include "../colors.hpp"
#include <libres.hpp>

namespace libview::objects
{

blank_button::blank_button
(
    object2d& parent,
    features::drawable_group& drawables,
    features::clickable_group& clickables,
    const style& stl,
    const callback_set& callbacks
):
    object2d{&parent},
    features::clickable{*this, &clickables},
    style_(stl),
    callbacks_(callbacks),
    background_rectangle_
    (
        *this,
        drawables,
        rounded_rectangle::style
        {
            .color = style_.color,
            .dimension = style_.dimension,
            .outline_color = style_.outline_color,
            .outline_thickness = style_.outline_thickness,
            .radius = style_.radius
        }
    )
{
}

void blank_button::set_enabled(const bool enabled)
{
    enabled_ = enabled;
}

bool blank_button::is_inside(const Magnum::Vector2& model_space_position) const
{
    if(!enabled_)
    {
        return false;
    }

    const auto x = model_space_position.x();
    const auto y = model_space_position.y();
    const auto xs = style_.dimension.x();
    const auto ys = style_.dimension.y();
    return -xs <= x && x <= xs && -ys <= y && y <= ys;
}

void blank_button::handle_mouse_press()
{
    background_rectangle_.set_color(style_.highlight_color);
    callbacks_.mouse_press_callback();
}

void blank_button::handle_mouse_release()
{
    background_rectangle_.set_color(style_.color);
    callbacks_.mouse_release_callback();
}

void blank_button::handle_mouse_enter()
{
    background_rectangle_.set_color(style_.highlight_color);
}

void blank_button::handle_mouse_leave()
{
    background_rectangle_.set_color(style_.color);
}

} //namespace
