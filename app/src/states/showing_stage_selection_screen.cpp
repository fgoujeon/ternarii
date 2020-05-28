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

#include "showing_stage_selection_screen.hpp"
#include "playing.hpp"
#include "showing_title_screen.hpp"

namespace states
{

showing_stage_selection_screen::showing_stage_selection_screen(fsm& ctx, const screen_transition trans):
    fsm_(ctx),
    pscreen_
    (
        fsm_.view.make_screen<screen>
        (
            screen::callback_set
            {
                .purity_room_selection_request = [this]{fsm_.set_state<playing>(screen_transition::right_to_left);},
                .nullifier_room_selection_request = [this]{fsm_.set_state<playing>(screen_transition::right_to_left);},
                .back_request = [this]{fsm_.set_state<showing_title_screen>(screen_transition::left_to_right);}
            }
        )
    )
{
    fsm_.view.show_screen(pscreen_, trans);
}

} //namespace
