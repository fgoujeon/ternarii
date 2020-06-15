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

#include "pause_overlay.hpp"
#include "../text.hpp"
#include "../styles.hpp"
#include "../colors.hpp"

namespace libview::objects
{

pause_overlay::pause_overlay
(
    object2d& parent,
    features::drawable_group& drawables,
    features::clickable_group& clickables,
    const callback_set& callbacks
):
    object2d{&parent},
    background_rectangle_
    (
        *this,
        drawables,
        colors::light_gray
    ),
    pause_label_
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
        "PAUSE"
    ),
    resume_button_
    (
        *this,
        drawables,
        clickables,
        styles::white_label_button,
        "RESUME",
        objects::label_button::callback_set
        {
            .mouse_release_callback = callbacks.handle_resume_request
        }
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
            .mouse_release_callback = callbacks.handle_exit_request
        }
    ),
    save_note_label_
    (
        *this,
        drawables,
        label::style
        {
            .alignment = Magnum::Text::Alignment::MiddleCenter,
            .color = colors::dark_gray,
            .font_size = 0.3f
        },
        "Note: Game is saved at each move."
    )
{
    background_rectangle_.scale({50.0f, 4.0f});

    pause_label_.translate({0.0f, 3.0f});

    resume_button_.scale({2.0f, 2.0f});
    resume_button_.translate({0.0f, -1.0f});

    exit_button_.scale({2.0f, 2.0f});
    exit_button_.translate({0.0f, -2.0f});

    save_note_label_.translate({0.0f, -3.0f});
}

} //namespace
