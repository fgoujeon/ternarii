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

#ifndef LIBGAME_EVENT_HPP
#define LIBGAME_EVENT_HPP

#include "data_types.hpp"
#include <variant>
#include <vector>
#include <memory>
#include <array>
#include <ostream>

namespace libgame
{

namespace events
{
    struct board_tile_drop
    {
        data_types::board_tile_drop_list drops;
    };

    std::ostream& operator<<(std::ostream& l, const board_tile_drop& r);



    struct end_of_game{};

    std::ostream& operator<<(std::ostream& l, const end_of_game&);



    struct hi_score_change
    {
        int score = 0;
    };

    std::ostream& operator<<(std::ostream& l, const hi_score_change& r);



    struct input_tile_drop
    {
        data_types::input_tile_drop_list drops;
    };

    std::ostream& operator<<(std::ostream& l, const input_tile_drop& r);



    struct move_count_change
    {
        int value = 0;
    };

    std::ostream& operator<<(std::ostream& l, const move_count_change& r);



    struct next_input_creation
    {
        data_types::input_tile_matrix tiles;
    };

    std::ostream& operator<<(std::ostream& l, const next_input_creation& r);



    struct next_input_insertion{};

    std::ostream& operator<<(std::ostream& l, const next_input_insertion& r);



    struct score_change
    {
        int score = 0;
    };

    std::ostream& operator<<(std::ostream& l, const score_change& r);



    struct start{};

    std::ostream& operator<<(std::ostream& l, const start&);



    struct tile_merge
    {
        data_types::tile_merge_list merges;
        data_types::granite_erosion_list granite_erosions;
    };

    std::ostream& operator<<(std::ostream& l, const tile_merge& r);



    struct tile_nullification
    {
        libutil::matrix_coordinate_list nullified_tile_coordinates;
    };

    std::ostream& operator<<(std::ostream& l, const tile_nullification& r);



    struct tile_value_change
    {
        libutil::matrix_coordinate nullified_tile_coordinate;
        data_types::tile_value_change_list changes;
    };

    std::ostream& operator<<(std::ostream& l, const tile_value_change& r);
}

using event = std::variant
<
    events::board_tile_drop,
    events::end_of_game,
    events::hi_score_change,
    events::input_tile_drop,
    events::move_count_change,
    events::next_input_creation,
    events::next_input_insertion,
    events::score_change,
    events::start,
    events::tile_merge,
    events::tile_nullification,
    events::tile_value_change
>;

using event_list = std::vector<event>;

} //namespace

#endif
