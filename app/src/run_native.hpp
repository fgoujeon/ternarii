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

#include "controller.hpp"
#include <SDL2/SDL.h>
#include <cassert>

void run()
{
    const unsigned int frame_duration = 1000 / 60;

    controller c;

    while(!c.must_quit())
    {
        const auto begin_time = SDL_GetTicks();
        c.iterate();
        const auto end_time = SDL_GetTicks();

        //regulate the frame rate
        assert(end_time >= begin_time);
        const auto elapsed_time = end_time - begin_time;
        if(elapsed_time < frame_duration)
        {
            const Uint32 remaining_time = frame_duration - elapsed_time;
            SDL_Delay(remaining_time);
        }
    }
}
