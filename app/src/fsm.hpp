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

#ifndef FSM_HPP
#define FSM_HPP

#include "events.hpp"
#include "context.hpp"
#include "states/playing.hpp"
#include "states/showing_about_screen.hpp"
#include "states/showing_stage_selection_screen.hpp"
#include "states/showing_title_screen.hpp"
#include <fgfsm.hpp>

//Simple states
namespace states
{
    struct idle { context& ctx; };
    struct loading_database { context& ctx; };
}

using fsm_transition_table = fgfsm::transition_table
<
    //         start state,                            event,                                       target state
    fgfsm::row<states::idle,                           events::start,                               states::loading_database>,
    fgfsm::row<states::loading_database,               libdb::events::end_of_loading,               states::showing_title_screen>,
    fgfsm::row<states::showing_title_screen,           events::stage_selection_screen_show_request, states::showing_stage_selection_screen>,
    fgfsm::row<states::showing_title_screen,           events::about_screen_show_request,           states::showing_about_screen>,
    fgfsm::row<states::showing_about_screen,           events::title_screen_show_request,           states::showing_title_screen>,
    fgfsm::row<states::showing_stage_selection_screen, events::title_screen_show_request,           states::showing_title_screen>,
    fgfsm::row<states::showing_stage_selection_screen, events::play_screen_show_request,            states::playing>,
    fgfsm::row<states::playing,                        events::stage_selection_screen_show_request, states::showing_stage_selection_screen>
>;

//Finite state machine
class fsm
{
    public:
        fsm(context& ctx):
            impl_(ctx)
        {
        }

        void process_event(const fgfsm::event_ref& event)
        {
            impl_.process_event(event);
        }

    private:
        using impl = fgfsm::fsm<fsm_transition_table>;
        impl impl_;
};

void process_event(fsm& sm, const fgfsm::event_ref& event);

#endif
