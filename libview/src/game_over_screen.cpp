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

#include "game_over_screen.hpp"

namespace libview
{

game_over_screen::game_over_screen(SDL_Renderer& renderer):
    renderer_(renderer),
    pfont_
    (
        TTF_OpenFont("res/fonts/DejaVuSans.ttf", 90)
    ),
    pgame_over_texture_
    (
        libsdl::make_texture
        (
            renderer_,
            *pfont_,
            "GAME OVER",
            SDL_Color{0x88, 0x88, 0x88, 255}
        )
    )
{
}

void game_over_screen::set_area(const SDL_Rect& area)
{
    area_ = area;
}

void game_over_screen::set_visible(const bool value)
{
    visible_ = value;
}

void game_over_screen::draw(SDL_Renderer& renderer)
{
    if(!visible_)
        return;

    //background
    SDL_SetRenderDrawColor(&renderer, 0x44, 0x44, 0x44, 255);
    SDL_RenderFillRect(&renderer, &area_);

    //"GAME OVER"
    {
        int texture_width_px;
        int texture_height_px;
        SDL_QueryTexture
        (
            pgame_over_texture_.get(),
            nullptr,
            nullptr,
            &texture_width_px,
            &texture_height_px
        );
        const auto texture_ratio =
            static_cast<double>(texture_width_px) /
            texture_height_px
        ;

        SDL_Rect r;
        r.w = area_.w;
        r.h = r.w / texture_ratio;
        r.x = area_.x + area_.w / 2 - r.w / 2;
        r.y = area_.y + area_.h / 2 - r.h / 2;

        SDL_RenderCopy(&renderer, pgame_over_texture_.get(), nullptr, &r);
    }
}

} //namespace view
