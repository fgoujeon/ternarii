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

#ifndef LIBVIEW_SCREENS_VERSUS_GAME_DETAIL_FSM_HPP
#define LIBVIEW_SCREENS_VERSUS_GAME_DETAIL_FSM_HPP

#include "../../animation.hpp"
#include "../../objects/tile_grid.hpp"
#include <libview/screens/versus_game.hpp>
#include <libview/data_types.hpp>
#include <libutil/fsm.hpp>
#include <chrono>

namespace libview::screens::versus_game_detail
{

//Events sent to states
namespace events
{
    struct p1_button_press
    {
        data_types::move_button button;
    };

    struct p2_button_press
    {
        data_types::move_button button;
    };

    struct p1_button_release
    {
        data_types::move_button button;
    };

    struct p2_button_release
    {
        data_types::move_button button;
    };

    struct game_over{};

    struct new_game_request{};

    struct pause_request{};

    struct iteration{};
}

struct fsm_context
{
    versus_game& screen;
    feature_group_set& feature_groups;
    versus_game::callback_set& callbacks;
    animation::animator& p1_animator;
    animation::animator& p2_animator;
    animation::animator& pause_animator;
    objects::tile_grid& p1_tile_grid;
    objects::tile_grid& p2_tile_grid;

    int time_s = 0;
    int move_count = 0;
    int hi_score = 0;
};

using fsm = libutil::fsm::fsm<fsm_context>;

} //namespace

#endif
