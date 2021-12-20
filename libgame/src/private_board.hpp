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

#ifndef PRIVATE_BOARD_HPP
#define PRIVATE_BOARD_HPP

#include <libgame/board.hpp>
#include <libgame/events.hpp>
#include <libgame/data_types.hpp>
#include <libgame/constants.hpp>
#include <vector>
#include <memory>

namespace libgame
{

class private_board
{
    public:
        static constexpr auto total_column_count      = constants::board_column_count;
        static constexpr auto total_row_count         = constants::board_row_count;
        static constexpr auto authorized_column_count = constants::board_authorized_column_count;
        static constexpr auto authorized_row_count    = constants::board_authorized_row_count;
        static constexpr auto authorized_cell_count   = constants::board_authorized_cell_count;

    public:
        private_board(board& brd);

        const board& tile_array() const
        {
            return board_;
        }

        libutil::matrix_coordinate_list get_targeted_tiles
        (
            const data_types::input_tile_matrix& input_tiles,
            const data_types::input_layout& input_layout
        ) const;

        void drop_input_tiles
        (
            const data_types::input_tile_matrix& input_tiles,
            const data_types::input_layout& input_layout,
            event_list& events
        );

    private:
        board& board_;
};

} //namespace

#endif
