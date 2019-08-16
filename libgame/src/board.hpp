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

#ifndef BOARD_HPP
#define BOARD_HPP

#include <libgame/events.hpp>
#include <libgame/data_types.hpp>
#include <vector>
#include <memory>

namespace libgame
{

class board_input;

class board
{
    public:
        static constexpr int total_column_count = 6;
        static constexpr int total_row_count = 10;

        //dimensions of the zone, starting from the bottom of the board, that can
        //take tiles without causing game over
        static constexpr int authorized_column_count = total_column_count;
        static constexpr int authorized_row_count = 7;
        static constexpr int authorized_cell_count = authorized_column_count * authorized_row_count;

        using grid_t = data_types::tile_grid<total_column_count, total_row_count>;

    public:
        board
        (
            grid_t& tiles,
            int& hi_score
        );

        const grid_t& tile_grid() const
        {
            return tile_grid_;
        }

        bool is_game_over() const;

        int get_highest_tile_value() const;

        int get_score() const;

        int get_free_cell_count() const;

        void clear();

        void drop_input(const board_input& in, event_list& events);

    private:
        events::input_insertion insert_input(const board_input& in);

        data_types::tile_drop_list make_tiles_fall();

        data_types::tile_merge_list merge_tiles();

        enum class selection_state
        {
            unselected,
            visited,
            selected
        };

        using selection_t = data_types::array2d<selection_state, total_column_count, total_row_count>;

        void select_tiles
        (
            const int tile_value,
            const int column_index,
            const int row_index,
            selection_t& selection,
            int& selection_size
        );

    private:
        grid_t& tile_grid_;
        int& hi_score_;
};

} //namespace

#endif
