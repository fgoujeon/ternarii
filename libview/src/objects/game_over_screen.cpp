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

#include "game_over_screen.hpp"
#include "../text.hpp"
#include "../styles.hpp"
#include "../colors.hpp"

namespace libview::objects
{

game_over_screen::game_over_screen
(
    Object2D& parent,
    features::drawable_group& drawables,
    features::clickable_group& clickables,
    const libutil::void_function<>& new_game_button_press_callback
):
    Object2D{&parent},
    background_rectangle_(*this, drawables, colors::light_gray),
    label_
    (
        *this,
        drawables,
        label::style
        {
            .alignment = Magnum::Text::Alignment::MiddleCenter,
            .color = colors::dark_gray,
            .font_size = 1.0f,
            .outline_color = colors::dark_gray,
            .outline_range = {0.48f, 0.5f}
        },
        "GAME OVER"
    ),
    new_game_button_
    (
        *this,
        drawables,
        clickables,
        styles::white_label_button,
        "NEW GAME",
        objects::label_button::callback_set
        {
            .mouse_release_callback = [new_game_button_press_callback]{new_game_button_press_callback();}
        }
    )
{
    background_rectangle_.scale({50.0f, 1.0f});

    label_.translate({0.0f, 0.4f});

    new_game_button_.scale({1.8f, 1.8f});
    new_game_button_.translate({0.0f, -0.5f});
}

} //namespace
