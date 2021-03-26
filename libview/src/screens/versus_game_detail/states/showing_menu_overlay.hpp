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

#ifndef LIBVIEW_SCREENS_VERSUS_GAME_DETAIL_STATES_SHOWING_MENU_OVERLAY_HPP
#define LIBVIEW_SCREENS_VERSUS_GAME_DETAIL_STATES_SHOWING_MENU_OVERLAY_HPP

#include "../fsm.hpp"
#include "../../../objects/game_menu_overlay.hpp"
#include <libview/data_types.hpp>
#include <libutil/fsm.hpp>

namespace libview::screens::versus_game_detail::states
{

class showing_menu_overlay: public libutil::fsm::state
{
    public:
        showing_menu_overlay(fsm& fsm);

        ~showing_menu_overlay();

        void handle_event(const std::any& event);

    private:
        fsm& fsm_;
        std::shared_ptr<objects::game_menu_overlay> pmenu_overlay_;
};

} //namespace

#endif
