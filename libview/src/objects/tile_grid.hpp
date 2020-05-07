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

#include "number_tile.hpp"
#include "sdf_image.hpp"
#include "../animation.hpp"
#include "../time.hpp"
#include "../magnum_common.hpp"
#include <libview/data_types.hpp>
#include <libutil/matrix.hpp>
#include <Magnum/Animation/Player.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/VertexColor.h>
#include <memory>

namespace libview::objects
{

class tile_grid: public Object2D
{
    private:
        using input_tile_array = libutil::matrix<std::shared_ptr<number_tile>, 2, 2>;
        using board_tile_array = libutil::matrix<std::shared_ptr<number_tile>, 6, 10>;

    public:
        explicit tile_grid(SceneGraph::DrawableGroup2D& drawables, Object2D& parent);

        bool is_animating() const;

        void clear();

        void create_next_input(const data_types::input_tile_array& tiles);

        void insert_next_input(const data_types::input_layout& layout);

        void set_input_layout(const data_types::input_layout& layout);

        void drop_input_tiles(const data_types::input_tile_drop_list& drops);

        void drop_board_tiles(const data_types::board_tile_drop_list& drops);

        void make_vertical_bomb_tiles_explode(const data_types::vertical_bomb_tile_explosion_list& explosions);

        void merge_tiles(const data_types::tile_merge_list& merges);

        void set_board_tiles(const data_types::board_tile_array& tiles);

        void advance(const time_point& now);

    private:
        std::shared_ptr<number_tile> make_number_tile
        (
            const int value,
            const Magnum::Vector2& position
        );

    private:
        SceneGraph::DrawableGroup2D& drawables_;

        animation_list animations_;

        std::vector<std::unique_ptr<sdf_image>> board_corners_;

        input_tile_array next_input_tiles_ = {};
        input_tile_array input_tiles_ = {};
        data_types::input_layout input_layout_;
        board_tile_array board_tiles_ = {};
};

} //namespace

#endif
