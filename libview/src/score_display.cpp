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

score_display::score_display(SDL_Renderer& renderer):
    renderer_(renderer),
    pfont_
    (
        TTF_OpenFont("res/fonts/DejaVuSans.ttf", 90)
    )
{
    set_score(0);
}

void score_display::set_area(const SDL_Rect& area)
{
    area_ = area;
}

void score_display::set_score(const unsigned int value)
{
    ptexture_ = libsdl::make_texture
    (
        renderer_,
        *pfont_,
        score_to_string(value),
        SDL_Color{255, 255, 255, 255}
    );
}

void score_display::draw(SDL_Renderer& renderer)
{
    int texture_width_px;
    int texture_height_px;
    SDL_QueryTexture
    (
        ptexture_.get(),
        nullptr,
        nullptr,
        &texture_width_px,
        &texture_height_px
    );
    const auto texture_ratio =
        static_cast<double>(texture_width_px) /
        texture_height_px
    ;

    const auto score_height_px = area_.h;
    const auto score_width_px = score_height_px * texture_ratio;

    SDL_Rect r;
    r.x = area_.x + area_.w - score_width_px;
    r.y = area_.y;
    r.w = score_width_px;
    r.h = score_height_px;

    SDL_RenderCopy(&renderer, ptexture_.get(), nullptr, &r);
}

} //namespace view
