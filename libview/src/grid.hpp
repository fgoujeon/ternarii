#ifndef LIBVIEW_GRID_HPP
#define LIBVIEW_GRID_HPP

#include "tile.hpp"
#include "animations.hpp"
#include <libview/data_types.hpp>
#include <libsdl.hpp>
#include <vector>
#include <array>
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


namespace libview
{

class grid
{
    private:
        template<size_t Size0, size_t Size1>
        using tile_array = std::array<std::array<std::unique_ptr<tile>, Size1>, Size0>;

        using next_input_tile_array = std::array<std::unique_ptr<tile>, 2>;
        using input_tile_array = std::array<std::unique_ptr<tile>, 2>;
        using board_tile_array = tile_array<6, 10>;

    public:
        grid(SDL_Renderer& renderer);

        int get_logical_width() const;

        int get_logical_height() const;

        void draw(SDL_Renderer& renderer);

        void create_next_input(const unsigned int value0, const unsigned int value1);

        void insert_next_input(const unsigned int x_offset, const unsigned int rotation);

        void set_next_input_items(const data_types::next_input_item_array& items);

        void set_input_items(const data_types::input_item_array& items);

        void set_input_x_offset(const unsigned int value);

        void set_input_rotation(const unsigned int value);

        void insert_input
        (
            const unsigned int tile0_dst_column_index,
            const unsigned int tile0_dst_row_index,
            const unsigned int tile1_dst_column_index,
            const unsigned int tile1_dst_row_index
        );

        void drop_tiles(const std::vector<data_types::tile_drop>& drops);

        void merge_tiles(const std::vector<data_types::tile_merge>& merges);

        void set_board_items(const data_types::board_item_array& items);

    private:
        next_input_tile_array next_input_tiles_;
        input_tile_array input_tiles_;
        unsigned int input_x_offset_ = 0;
        unsigned int input_rotation_ = 0;
        board_tile_array board_tiles_;
        std::vector<std::unique_ptr<tile>> disappearing_tiles_;
        animation_queue animations_;
};

} //namespace libview

#endif
