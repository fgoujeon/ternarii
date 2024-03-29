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

#ifndef LIBVIEW_SCREENS_GAME_DETAIL_STATES_SHOWING_MENU_OVERLAY_HPP
#define LIBVIEW_SCREENS_GAME_DETAIL_STATES_SHOWING_MENU_OVERLAY_HPP

#include "../context.hpp"
#include "../../../objects/game_menu_overlay.hpp"
#include <libview/data_types.hpp>
#include <fgfsm.hpp>

namespace libview::screens::game_detail
{

class showing_menu_overlay
{
    public:
        struct resume_request{};

        showing_menu_overlay(context& ctx);

        void on_entry();

        void on_event(const fgfsm::event_ref& event);

        void on_exit();

    private:
        context& ctx_;
        std::shared_ptr<objects::game_menu_overlay> pmenu_overlay_;
};

} //namespace

#endif
