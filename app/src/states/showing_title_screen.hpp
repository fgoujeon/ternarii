/*
Copyright 2018 - 2021 Florian Goujeon

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

#ifndef STATES_SHOWING_TITLE_SCREEN_HPP
#define STATES_SHOWING_TITLE_SCREEN_HPP

#include "showing_stage_selection_screen.hpp"
#include "showing_about_screen.hpp"
#include "playing_versus.hpp"
#include "../fsm.hpp"
#include <libview/screens/title.hpp>

namespace states
{

class showing_title_screen final: public libutil::fsm::state
{
    private:
        using screen = libview::screens::title;
        using screen_transition = libview::view::screen_transition;

    public:
        showing_title_screen(fsm& ctx, const libview::view::screen_transition trans):
            fsm_(ctx),
            pscreen_
            (
                fsm_.get_context().view.make_screen<libview::screens::title>
                (
                    screen::callback_set
                    {
                        .play_request  = [this]
                        {
                            fsm_.set_state<showing_stage_selection_screen>
                            (
                                screen_transition::right_to_left
                            );
                        },
                        .about_request = [this]
                        {
                            fsm_.set_state<showing_about_screen>
                            (
                                screen_transition::right_to_left
                            );
                        },
                        .versus_request = [this]
                        {
                            fsm_.set_state<playing_versus>
                            (
                                screen_transition::right_to_left,
                                libview::data_types::stage::nullifier_room
                            );
                        }
                    }
                )
            )
        {
            fsm_.get_context().view.show_screen(pscreen_, trans);
        }

    private:
        fsm& fsm_;
        std::shared_ptr<screen> pscreen_;
};

} //namespace

#endif
