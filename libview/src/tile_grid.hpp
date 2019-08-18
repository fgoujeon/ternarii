/*
Copyright 2018 - 2019 Florian Goujeon

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

#ifndef LIBVIEW_TILE_GRID_HPP
#define LIBVIEW_TILE_GRID_HPP

#include "tile.hpp"
#include "magnum_common.hpp"
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/VertexColor.h>

namespace libview
{

class tile_grid: public Object2D
{
    private:
        template<size_t Size0, size_t Size1>
        using tile_array = std::array<std::array<tile*, Size1>, Size0>;

        using next_input_tile_array = std::array<tile*, 2>;
        using input_tile_array = std::array<tile*, 2>;
        using board_tile_array = tile_array<6, 10>;

    public:
        explicit tile_grid(SceneGraph::DrawableGroup2D& drawables, Object2D* parent);

        void create_next_input(const unsigned int value0, const unsigned int value1);

        void insert_next_input(const unsigned int x_offset, const unsigned int rotation);

        void set_input_x_offset(const unsigned int value);

        void set_input_rotation(const unsigned int value);

    private:
        void update_input_tiles_positions();

    private:
        SceneGraph::DrawableGroup2D& drawables_;

        next_input_tile_array next_input_tiles_;
        input_tile_array input_tiles_;
        unsigned int input_x_offset_ = 0;
        unsigned int input_rotation_ = 0;
        board_tile_array board_tiles_;
};

} //namespace

#endif
