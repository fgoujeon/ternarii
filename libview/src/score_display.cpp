/*
Copyright 2018 Florian Goujeon

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

#include "score_display.hpp"

namespace libview
{

namespace
{
    std::string score_to_string(const unsigned int score)
    {
        std::string str;
        auto temp_score = score;
        auto digit_index = 0;

        do
        {
            const auto digit = temp_score % 10;
            const auto digit_char = static_cast<char>('0' + digit);

            //add thousands separator
            if(digit_index != 0 && digit_index % 3 == 0)
                str = ' ' + str;

            //add digit
            str = std::string{digit_char} + str;

            ++digit_index;
            temp_score /= 10;
        } while(temp_score != 0);

        return str;
    }
}

score_display::score_display
(
    SDL_Renderer& renderer,
    const SDL_Rect& area
):
    label_
    (
        renderer,
        "res/fonts/DejaVuSans.ttf",
        area.h,
        SDL_Color{0xff, 0xff, 0xff, 0xff},
        point
        {
            static_cast<double>(area.x),
            static_cast<double>(area.y)
        },
        area.w,
        area.h,
        "0",
        horizontal_alignment::right,
        vertical_alignment::center
    )
{
}

void score_display::set_score(const unsigned int value)
{
    label_.set_text(score_to_string(value));
}

void score_display::draw()
{
    label_.draw();
}

} //namespace view
