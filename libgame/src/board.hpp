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

#include <libgame/events.hpp>
#include <libgame/data_types.hpp>
#include <libgame/constants.hpp>
#include <vector>
#include <memory>

namespace libgame
{

class board
{
    public:
        using event = std::variant
        <
            events::board_tile_drop,
            events::input_tile_drop,
            events::score_change,
            events::tile_merge,
            events::tile_nullification
        >;

        using event_list = std::vector<event>;

        static constexpr auto total_column_count      = constants::board_column_count;
        static constexpr auto total_row_count         = constants::board_row_count;
        static constexpr auto authorized_column_count = constants::board_authorized_column_count;
        static constexpr auto authorized_row_count    = constants::board_authorized_row_count;
        static constexpr auto authorized_cell_count   = constants::board_authorized_cell_count;

        board(data_types::board_tile_matrix& tiles);

        const data_types::board_tile_matrix& tile_array() const
        {
            return tiles_;
        }

        void get_targeted_tiles
        (
            const data_types::input_tile_matrix& input_tiles,
            const data_types::input_layout& input_layout,
            libutil::matrix_coordinate_list& coords
        ) const;

        void drop_input_tiles
        (
            const data_types::input_tile_matrix& input_tiles,
            const data_types::input_layout& input_layout,
            event_list& events
        );

    private:
        data_types::board_tile_drop_list make_tiles_fall();

        data_types::tile_merge_list merge_tiles();

        enum class selection_state
        {
            unselected,
            visited,
            selected
        };

        using selection_t = libutil::matrix<selection_state, total_column_count, total_row_count>;

        void select_tiles
        (
            const int tile_value,
            const int col,
            const int row,
            selection_t& selection,
            int& selection_size
        );

        data_types::board_tile_matrix& tiles_;
};

} //namespace

#endif
