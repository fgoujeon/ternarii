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

#ifndef BOARD_HPP
#define BOARD_HPP

#include "data_types.hpp"
#include "events.hpp"

namespace libgame
{

struct board
{
    data_types::board_tile_matrix tiles;
};

int get_tile_count(const board& brd);

//Get the row index of the lowest empty cell on the given column, if any.
std::optional<int> get_lowest_empty_cell
(
    const board& brd,
    const int col
);

bool is_overflowed(const board& brd);

int get_highest_tile_value(const board& brd);

int get_score(const board& brd);



struct apply_gravity_on_input_result
{
    board brd;
    data_types::input_tile_drop_list drops;
};

/*
Drop input tiles on board
*/
apply_gravity_on_input_result apply_gravity_on_input
(
    const board& brd,
    const data_types::input_tile_matrix& input_tiles,
    const data_types::input_layout& input_layout
);



struct apply_gravity_result
{
    board brd;
    data_types::board_tile_drop_list drops;
};

/*
Apply gravity on board tiles
*/
apply_gravity_result apply_gravity(const board& brd);



struct apply_nullifiers_result
{
    board brd;
    libutil::matrix_coordinate_list nullified_tiles_coords;
};

/*
Apply nullifier tiles on board tiles
*/
apply_nullifiers_result apply_nullifiers(const board& brd);



board apply_adders
(
    board brd,
    event_list& events
);

struct apply_merges_on_granites_result
{
    board brd;
    data_types::granite_erosion_list granite_erosions;
};

/*
Decrement the thickness of granites whose adjacent tiles contain a number
tile involved in a merge.
*/
apply_merges_on_granites_result apply_merges_on_granites
(
    const board& brd,
    const data_types::tile_merge_list& merges
);



namespace data_types
{
    struct stage_state
    {
        double time_s = 0;
        int hi_score = 0;
        int move_count = 0;
        input_tile_matrix next_input_tiles;
        input_tile_matrix input_tiles;
        board brd;
    };
}

} //namespace

#endif
