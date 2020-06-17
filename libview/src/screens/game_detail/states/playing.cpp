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

#include "playing.hpp"
#include "showing_game_over_overlay.hpp"
#include "showing_menu_overlay.hpp"

namespace libview::screens::game_detail::states
{

void playing::handle_event(const std::any& event)
{
    if(const auto pevent = std::any_cast<events::button_press>(&event))
    {
        fsm_.get_context().tile_grid.handle_button_press(pevent->button);
    }
    else if(const auto pevent = std::any_cast<events::button_release>(&event))
    {
        fsm_.get_context().tile_grid.handle_button_release(pevent->button);
    }
    else if(std::any_cast<events::game_over>(&event))
    {
        fsm_.set_state<showing_game_over_overlay>();
    }
    else if(std::any_cast<events::pause_request>(&event))
    {
        fsm_.set_state<showing_menu_overlay>();
    }
}

} //namespace
