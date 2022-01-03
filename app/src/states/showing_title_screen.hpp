/*
Copyright 2018 - 2022 Florian Goujeon

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

#include "../context.hpp"
#include <libview/screens/title.hpp>

namespace states
{

struct showing_title_screen
{
    void on_entry(const fgfsm::event_ref& event)
    {
        auto processed = false;

        visit
        (
            event,

            [this, &processed](const events::title_screen_show_request& event)
            {
                using screen = libview::screens::title;
                auto pscreen = ctx.view.make_screen<screen>
                (
                    screen::callback_set
                    {
                        .play_request  = [this]
                        {
                            ctx.process_event
                            (
                                events::stage_selection_screen_show_request
                                {
                                    data_types::screen_transitions::right_to_left{}
                                }
                            );
                        },
                        .about_request = [this]
                        {
                            ctx.process_event
                            (
                                events::about_screen_show_request
                                {
                                    data_types::screen_transitions::right_to_left{}
                                }
                            );
                        }
                    }
                );
                ctx.view.show_screen(pscreen, event.transition);

                processed = true;
            }
        );

        if(!processed)
        {
            on_entry
            (
                events::title_screen_show_request
                {
                    data_types::screen_transitions::top_to_bottom{}
                }
            );
        }
    }

    context& ctx;
};

} //namespace

#endif
