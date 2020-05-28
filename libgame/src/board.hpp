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
#include <libcommon/constants.hpp>
#include <vector>
#include <memory>

namespace libgame
{

class board_input;

class board
{
    public:
        static constexpr auto total_column_count      = libcommon::constants::board_column_count;
        static constexpr auto total_row_count         = libcommon::constants::board_row_count;
        static constexpr auto authorized_column_count = libcommon::constants::board_authorized_column_count;
        static constexpr auto authorized_row_count    = libcommon::constants::board_authorized_row_count;
        static constexpr auto authorized_cell_count   = libcommon::constants::board_authorized_cell_count;

    public:
        board(data_types::board_tile_matrix& tiles);

        const data_types::board_tile_matrix& tile_array() const
        {
            return tiles_;
        }

        void get_targeted_tiles(const board_input& in, libutil::matrix_coordinate_list& coords) const;

        void clear();

        void drop_input_tiles(const board_input& in, event_list& events);

    private:
        data_types::board_tile_drop_list make_tiles_fall();

        data_types::tile_merge_list merge_tiles();

        enum class selection_state
        {
            unselected,
            visited,
            selected
        };

        using selection_t = libutil::matrix<selection_state, total_row_count, total_column_count>;

        void select_tiles
        (
            const int tile_value,
            const int row,
            const int col,
            selection_t& selection,
            int& selection_size
        );

    private:
        data_types::board_tile_matrix& tiles_;
};

} //namespace

#endif
