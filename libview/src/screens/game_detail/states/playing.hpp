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

#ifndef LIBVIEW_SCREENS_GAME_DETAIL_STATES_PLAYING_HPP
#define LIBVIEW_SCREENS_GAME_DETAIL_STATES_PLAYING_HPP

#include "../events.hpp"
#include "../context.hpp"
#include <libview/data_types.hpp>
#include <fgfsm.hpp>

namespace libview::screens::game_detail
{

struct playing
{
    struct game_over_overlay_show_request{};
    struct menu_overlay_show_request{};

    void on_event(const fgfsm::event_ref& event)
    {
        visit
        (
            event,

            [this](const events::button_press& event)
            {
                ctx.tile_grid.handle_button_press(event.button);
            },

            [this](const events::button_release& event)
            {
                ctx.tile_grid.handle_button_release(event.button);
            },

            [this](const events::game_over&)
            {
                ctx.process_event(game_over_overlay_show_request{});
            },

            [this](const events::pause_request&)
            {
                ctx.process_event(menu_overlay_show_request{});
            }
        );
    }

    context& ctx;
};

} //namespace

#endif
