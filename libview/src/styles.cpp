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

#include "styles.hpp"

namespace libview::styles
{

auto title_label = objects::label::style
{
    .alignment = Magnum::Text::Alignment::MiddleCenter,
    .color = colors::light_gray,
    .font_size = 0.6f,
    .outline_color = colors::dark_gray,
    .outline_range = {0.47f, 0.44f}
};

auto body_label = objects::label::style
{
    .alignment = Magnum::Text::Alignment::MiddleCenter,
    .color = colors::light_gray,
    .font_size = 0.3f,
    .outline_color = colors::dark_gray,
    .outline_range = {0.47f, 0.40f}
};

auto white_label_button = objects::label_button::style
{
    .label =
    {
        .alignment = Magnum::Text::Alignment::MiddleCenter,
        .color = colors::dark_gray,
        .font_size = 0.2f
    },
    .button =
    {
        .color = colors::light_gray,
        .dimension = {1.0f, 0.2f},
        .highlight_color = colors::white,
        .outline_color = colors::dark_gray,
        .outline_thickness = 0.01f,
        .radius = 0.16f
    }
};

} //namespace
