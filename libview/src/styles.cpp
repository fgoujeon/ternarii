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

objects::label_button::style white_label_button
{
    .label = objects::label::style
    {
        .alignment = Magnum::Text::Alignment::MiddleCenter,
        .font_size = 0.2f,
        .color = colors::dark_gray,
        .outline_color = colors::dark_gray,
        .outline_range = {0.6f, 0.5f}
    },
    .color = colors::light_gray,
    .highlight_color = colors::white,
    .outline_color = colors::black,
    .radius = 0.16f
};

} //namespace
