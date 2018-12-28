/*
Copyright 2018 Florian Goujeon

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

#include <libgame_2p/events.hpp>
#include <libgame_2p/data_types.hpp>
#include <vector>
#include <memory>

namespace libgame_2p
{

class board_input;

class board
{
    public:
        static const unsigned int column_count = 6;
        static const unsigned int row_count = 10;
        using grid_t = data_types::tile_grid<column_count, row_count>;

        board();

        const grid_t& tile_grid() const
        {
            return tile_grid_;
        }

        bool is_game_over() const;

        unsigned int get_highest_tile_value() const
        {
            return highest_tile_value_;
        }

        unsigned int get_score() const;

        void clear();

        std::vector<event> drop_input(const board_input& in);

    private:
        events::input_insertion insert_input(const board_input& in);

        data_types::tile_drop_list make_tiles_fall();

        data_types::tile_merge_list merge_tiles();

        enum class selection_state
        {
            UNSELECTED,
            VISITED,
            SELECTED
        };

        typedef data_types::array2d<selection_state, column_count, row_count> selection_t;

        void select_tiles
        (
            const unsigned int tile_value,
            const unsigned int column_index,
            const unsigned int row_index,
            selection_t& selection,
            unsigned int& selection_size
        );

    private:
        grid_t tile_grid_;
        unsigned int highest_tile_value_;
};

} //namespace libgame_2p

#endif
