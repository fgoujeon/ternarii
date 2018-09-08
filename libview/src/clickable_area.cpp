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

#include "clickable_area.hpp"
#include <iostream>

namespace libview
{

clickable_area::clickable_area(const click_event_handler& evt_handler):
    evt_handler_(evt_handler)
{
    SDL_AddEventWatch(&static_process_event, this);
}

clickable_area::~clickable_area()
{
    SDL_DelEventWatch(&static_process_event, this);
}

void clickable_area::set_area(const SDL_Rect& area)
{
    area_ = area;
}

void clickable_area::draw(SDL_Renderer& renderer)
{
    SDL_SetRenderDrawColor(&renderer, 0xff, 0xff, 0xff, 0x40);
    SDL_RenderFillRect(&renderer, &area_);
}

int clickable_area::static_process_event
(
    void* pdata,
    SDL_Event* pevent
)
{
    auto pself = reinterpret_cast<clickable_area*>(pdata);
    auto& self = *pself;
    auto& event = *pevent;

    self.process_event(event);

    return 0;
}

void clickable_area::process_event(SDL_Event& event)
{
    if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        const auto click_position = SDL_Point{event.button.x, event.button.y};
        if(SDL_PointInRect(&click_position, &area_))
            evt_handler_();
    }
}

} //namespace libview