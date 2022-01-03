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

#ifndef LIBVIEW_SCREENS_GAME_DETAIL_FSM_HPP
#define LIBVIEW_SCREENS_GAME_DETAIL_FSM_HPP

#include "states/playing.hpp"
#include "states/showing_game_over_overlay.hpp"
#include "states/showing_menu_overlay.hpp"
#include <fgfsm.hpp>

namespace libview::screens::game_detail
{

using fsm_transition_table = fgfsm::transition_table
<
    fgfsm::row<playing,                   playing::menu_overlay_show_request,      showing_menu_overlay>,
    fgfsm::row<showing_menu_overlay,      showing_menu_overlay::resume_request,    playing>,
    fgfsm::row<playing,                   playing::game_over_overlay_show_request, showing_game_over_overlay>,
    fgfsm::row<showing_game_over_overlay, events::new_game_request,                playing>
>;

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

} //namespace

#endif
