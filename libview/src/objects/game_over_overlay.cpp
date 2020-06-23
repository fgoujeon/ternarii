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

#include "game_over_overlay.hpp"
#include "../text.hpp"
#include "../styles.hpp"
#include "../colors.hpp"
#include <libutil/to_string.hpp>

namespace libview::objects
{

game_over_overlay::game_over_overlay
(
    object2d& parent,
    features::drawable_group& drawables,
    features::clickable_group& clickables,
    int move_count,
    int time_s,
    const callback_set& callbacks
):
    object2d{&parent},
    background_rectangle_(*this, drawables, colors::light_gray),
    title_label_
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
    stat_label_
    (
        *this,
        drawables,
        label::style
        {
            .alignment = Magnum::Text::Alignment::MiddleCenter,
            .color = colors::dark_gray,
            .font_size = 0.4f,
            .outline_color = colors::dark_gray,
            .outline_range = {0.5f, 0.5f}
        },
        libutil::to_string(move_count) + " moves played in " +
        libutil::to_string(std::chrono::seconds(time_s))
    ),
    exit_button_
    (
        *this,
        drawables,
        clickables,
        styles::white_label_button,
        "EXIT",
        objects::label_button::callback_set
        {
            .mouse_release_callback = callbacks.handle_exit_button_press
        }
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
            .mouse_release_callback = callbacks.handle_new_game_button_press
        }
    )
{
    background_rectangle_.scale({50.0f, 1.5f});

    title_label_.translate({0.0f, 0.75f});

    stat_label_.translate({0.0f, -0.1f});

    exit_button_.scale({1.8f, 1.8f});
    exit_button_.translate({-2.0f, -0.85f});

    new_game_button_.scale({1.8f, 1.8f});
    new_game_button_.translate({2.0f, -0.85f});
}

} //namespace
