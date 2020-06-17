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

#ifndef LIBVIEW_SCREENS_GAME_DETAIL_FSM_HPP
#define LIBVIEW_SCREENS_GAME_DETAIL_FSM_HPP

#include "../../animation.hpp"
#include "../../objects/tile_grid.hpp"
#include <libview/screens/game.hpp>
#include <libutil/fsm.hpp>
#include <chrono>

namespace libview::screens::game_detail
{

struct fsm_context
{
    game& screen;
    feature_group_set& feature_groups;
    game::callback_set& callbacks;
    animation::animator& animator;
    animation::animator& pause_animator;
    objects::tile_grid& tile_grid;

    std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
    int move_count = 0;
    int hi_score = 0;
};

using fsm = libutil::fsm::fsm<fsm_context>;

} //namespace

#endif
