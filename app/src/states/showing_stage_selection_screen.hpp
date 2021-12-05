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

#ifndef STATES_SHOWING_STAGE_SELECTION_SCREEN_HPP
#define STATES_SHOWING_STAGE_SELECTION_SCREEN_HPP

#include "../context.hpp"
#include <libview/screens/stage_selection.hpp>

namespace states
{

struct showing_stage_selection_screen
{
    void on_entry(const fgfsm::event_ref& event)
    {
        visit
        (
            event,

            [this](const events::stage_selection_screen_show_request& event)
            {
                using screen = libview::screens::stage_selection;
                auto pscreen = ctx.view.make_screen<screen>
                (
                    screen::callback_set
                    {
                        .stage_selection_request = [this](const libview::data_types::stage stage)
                        {
                            ctx.process_event
                            (
                                events::play_screen_show_request
                                {
                                    screen_transition::zoom_in,
                                    stage
                                }
                            );
                        },
                        .back_request = [this]
                        {
                            ctx.process_event
                            (
                                events::title_screen_show_request
                                {
                                    screen_transition::left_to_right
                                }
                            );
                        }
                    }
                );
                ctx.view.show_screen(pscreen, event.transition);
            }
        );
    }

    context& ctx;
};

} //namespace

#endif
