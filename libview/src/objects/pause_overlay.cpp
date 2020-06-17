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
#include <libutil/to_string.hpp>
#include <cmath>

namespace libview::objects
{

namespace
{
    const auto name_label_style = label::style
    {
        .alignment = Magnum::Text::Alignment::LineRight,
        .color = colors::dark_gray,
        .font_size = 0.4f,
        .outline_range = {0.55f, 1.0f}
    };

    const auto value_label_style = label::style
    {
        .alignment = Magnum::Text::Alignment::LineLeft,
        .color = colors::dark_gray,
        .font_size = 0.4f,
        .outline_range = {0.5f, 1.0f}
    };
}

pause_overlay::pause_overlay
(
    object2d& parent,
    features::drawable_group& drawables,
    features::clickable_group& clickables,
    const callback_set& callbacks
):
    object2d{&parent},
    triangle_(*this, drawables, colors::light_gray),
    background_rectangle_
    (
        *this,
        drawables,
        rounded_rectangle::style
        {
            .color = colors::light_gray,
            .dimension = {1.0f, 0.75f},
            .radius = 0.16
        }
    ),
    time_name_label_        (*this, drawables, name_label_style,  "TIME:"),
    time_value_label_       (*this, drawables, value_label_style, "0"),
    move_count_name_label_  (*this, drawables, name_label_style,  "MOVES:"),
    move_count_value_label_ (*this, drawables, value_label_style, "0"),
    hi_score_name_label_    (*this, drawables, name_label_style,  "HI-SCORE:"),
    hi_score_value_label_   (*this, drawables, value_label_style, "0"),
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
            .alignment = Magnum::Text::Alignment::LineCenter,
            .color = colors::dark_gray,
            .font_size = 0.3f
        },
        "Note: Game is saved at each move."
    )
{
    triangle_.setScaling({0.25f, 0.25f});
    triangle_.setTranslation({-2.85f, 2.9f});
    triangle_.rotate(45.0_degf);

    background_rectangle_.setScaling({4.0f, 4.0f});

    time_name_label_.setTranslation({0.0f, 2.25f});
    time_value_label_.setTranslation({0.1f, 2.25f});

    move_count_name_label_.setTranslation({0.0f, 1.5f});
    move_count_value_label_.setTranslation({0.1f, 1.5f});

    hi_score_name_label_.setTranslation({0.0f, 0.75f});
    hi_score_value_label_.setTranslation({0.1f, 0.75f});

    resume_button_.setScaling({2.0f, 2.0f});
    resume_button_.setTranslation({0.0f, -0.5f});

    exit_button_.setScaling({2.0f, 2.0f});
    exit_button_.setTranslation({0.0f, -1.5f});

    save_note_label_.setTranslation({0.0f, -2.5f});
}

void pause_overlay::set_start_time(const std::chrono::system_clock::time_point& value)
{
    const auto duration = std::chrono::duration_cast<std::chrono::seconds>
    (
        std::chrono::system_clock::now() - value
    );
    time_value_label_.set_text(libutil::to_string(duration));
}

void pause_overlay::set_move_count(int value)
{
    move_count_value_label_.set_text(libutil::to_string(value));
}

void pause_overlay::set_hi_score(int value)
{
    hi_score_value_label_.set_text(libutil::to_string(value));
}

} //namespace
