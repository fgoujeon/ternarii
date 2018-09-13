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

#include "label_button.hpp"
#include "draw.hpp"
#include <iostream>

namespace libview
{

label_button::label_button
(
    SDL_Renderer& renderer,
    const SDL_Rect& area,
    const unsigned int font_size,
    const std::string& text,
    const click_event_handler& evt_handler
):
    renderer_(renderer),
    clickable_area_(area, evt_handler),
    label_
    (
        renderer,
        "res/fonts/DejaVuSans.ttf",
        font_size,
        SDL_Color{0xff, 0xff, 0xff, 0xff},
        point{static_cast<double>(area.x), static_cast<double>(area.y)},
        area.w,
        area.h,
        text,
        horizontal_alignment::center,
        vertical_alignment::center
    )
{
}

void label_button::draw(const system& sys)
{
    //background
    {
        if(clickable_area_.is_clicked())
            SDL_SetRenderDrawColor(&renderer_, 0xff, 0xff, 0xff, 0x80);
        else if(clickable_area_.is_hovered())
            SDL_SetRenderDrawColor(&renderer_, 0xff, 0xff, 0xff, 0x60);
        else
            SDL_SetRenderDrawColor(&renderer_, 0xff, 0xff, 0xff, 0x40);

        draw_rect(renderer_, sys, clickable_area_.get_area());
    }

    label_.draw(sys);
}

} //namespace view
