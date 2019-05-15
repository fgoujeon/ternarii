/*
Copyright 2018 - 2019 Florian Goujeon

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
#include "draw.hpp"
#include <iostream>

namespace libview
{

game_over_screen::game_over_screen
(
    const event_handler& evt_handler,
    SDL_Renderer& renderer,
    const SDL_Rect& area
):
    renderer_(renderer),
    area_(area),
    game_over_label_
    (
        renderer,
        "res/fonts/DejaVuSans.ttf",
        100,
        SDL_Color{0x88, 0x88, 0x88, 0xff},
        geometry::point
        {
            static_cast<double>(area.x),
            static_cast<double>(area.y)
        },
        area.w,
        area.h,
        "GAME OVER",
        horizontal_alignment::center,
        vertical_alignment::top
    ),
    restart_button_
    (
        renderer,
        SDL_Rect
        {
            area.x + 200,
            area.y + 120,
            200,
            70
        },
        50,
        "restart",
        [this, evt_handler]
        {
            if(visible_)
                evt_handler(events::clear_request{});
        }
    )
{
}

void game_over_screen::set_visible(const bool value)
{
    visible_ = value;
}

void game_over_screen::draw(SDL_Renderer& renderer, const geometry::system& sys)
{
    if(!visible_)
        return;

    //background
    SDL_SetRenderDrawColor(&renderer, 0x44, 0x44, 0x44, 255);
    draw_rect(renderer, sys, area_);

    game_over_label_.draw(sys);
    restart_button_.draw(sys);
}

} //namespace view
