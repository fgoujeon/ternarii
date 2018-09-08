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
    restart_clickable_area_
    (
        SDL_Rect
        {
            area.x + 200,
            area.y + 120,
            200,
            70
        },
        [this, evt_handler]
        {
            if(visible_)
            {
                evt_handler(events::clear_request{});
                std::cout << "clicked\n";
            }
        }
    ),
    game_over_label_
    (
        renderer,
        point
        {
            static_cast<double>(area.x),
            static_cast<double>(area.y)
        },
        area.w,
        area.h,
        "GAME OVER",
        horizontal_alignment::center,
        vertical_alignment::top,
        "res/fonts/DejaVuSans.ttf",
        SDL_Color{0x88, 0x88, 0x88, 0xff}
    ),
    restart_label_
    (
        renderer,
        point
        {
            static_cast<double>(restart_clickable_area_.get_area().x + 10),
            static_cast<double>(restart_clickable_area_.get_area().y)
        },
        restart_clickable_area_.get_area().w - 20,
        restart_clickable_area_.get_area().h,
        "RESTART",
        horizontal_alignment::center,
        vertical_alignment::center,
        "res/fonts/DejaVuSans.ttf",
        SDL_Color{0xff, 0xff, 0xff, 0xff}
    )
{
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

    //restart button background
    {
        SDL_SetRenderDrawColor(&renderer, 0xff, 0xff, 0xff, 0x40);
        SDL_RenderFillRect(&renderer, &restart_clickable_area_.get_area());
    }

    game_over_label_.draw();
    restart_label_.draw();
}

} //namespace view
