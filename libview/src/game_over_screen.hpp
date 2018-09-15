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

#ifndef LIBVIEW_GAME_OVER_SCREEN_HPP
#define LIBVIEW_GAME_OVER_SCREEN_HPP

#include "label_button.hpp"
#include "label.hpp"
#include "system.hpp"
#include <libview/events.hpp>
#include <libsdl.hpp>

namespace libview
{

class game_over_screen
{
    public:
        game_over_screen
        (
            const event_handler& evt_handler,
            SDL_Renderer& renderer,
            const SDL_Rect& area
        );

        void set_visible(const bool value);

        void draw(SDL_Renderer& renderer, const system& sys);

    private:
        SDL_Renderer& renderer_;
        SDL_Rect area_;
        label game_over_label_;
        label_button restart_button_;
        bool visible_ = false;
};

} //namespace libview

#endif
