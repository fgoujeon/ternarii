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

#ifndef LIBVIEW_OBJECTS_TILE_GRID_HPP
#define LIBVIEW_OBJECTS_TILE_GRID_HPP

#include "tile_grid_detail/input.hpp"
#include "tile_grid_detail/next_input.hpp"
#include "number_tile.hpp"
#include "sdf_image_tile.hpp"
#include "sdf_image.hpp"
#include "../animation.hpp"
#include "../common.hpp"
#include <libview/data_types.hpp>
#include <libcommon/constants.hpp>
#include <libutil/matrix.hpp>
#include <libutil/time.hpp>
#include <Magnum/Animation/Player.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/VertexColor.h>
#include <memory>

namespace libview::objects
{

class tile_grid: public Object2D, public features::animable
{
    private:
        using input_tile_matrix = libutil::matrix
        <
            std::shared_ptr<Object2D>,
            libcommon::constants::input_column_count,
            libcommon::constants::input_row_count
        >;

        using board_tile_matrix = libutil::matrix
        <
            std::shared_ptr<Object2D>,
            libcommon::constants::board_column_count,
            libcommon::constants::board_row_count
        >;

    public:
        tile_grid
        (
            Object2D& parent,
            features::drawable_group& drawables,
            features::animable_group& animables,
            features::key_event_handler_group& key_event_handlers
        );

        bool is_animating() const;

        void clear();

        void create_next_input(const data_types::input_tile_matrix& tiles);

        void insert_next_input(const data_types::input_layout& layout);

        void set_input_layout(const data_types::input_layout& layout);

        void drop_input_tiles(const data_types::input_tile_drop_list& drops);

        void drop_board_tiles(const data_types::board_tile_drop_list& drops);

        void nullify_tiles(const libutil::matrix_coordinate_list& nullified_tile_coordinates);

        void merge_tiles(const data_types::tile_merge_list& merges);

        void mark_tiles_for_nullification(const libutil::matrix_coordinate_list& tile_coordinates);

        void set_board_tiles(const data_types::board_tile_matrix& tiles);

        void advance(const libutil::time_point& now, float elapsed_s);

    private:
        std::shared_ptr<Object2D> make_tile
        (
            const data_types::tile& tile,
            const Magnum::Vector2& position
        );

    private:
        features::drawable_group& drawables_;

        animator animator_;

        std::vector<std::unique_ptr<sdf_image>> board_corners_;
        data_types::input_layout input_layout_;
        board_tile_matrix board_tiles_ = {};

        tile_grid_detail::next_input next_input_;
        tile_grid_detail::input input_;
};

} //namespace

#endif
