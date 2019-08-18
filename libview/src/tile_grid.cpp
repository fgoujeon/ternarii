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

#include "tile_grid.hpp"
#include "tile.hpp"

namespace libview
{

namespace
{
    Magnum::Vector2 tile_coordinate_to_position(const data_types::tile_coordinate& c)
    {
        return
        {
            -2.5f + c.x,
            -5.0f + c.y
        };
    }

    std::array<Magnum::Vector2, 2> get_input_tile_positions(const unsigned int x_offset, const unsigned int rotation)
    {
        auto tile0_x = 0.0f;
        auto tile0_y = 0.0f;
        auto tile1_x = 0.0f;
        auto tile1_y = 0.0f;

        switch(rotation)
        {
            case 0:
                tile0_x = -0.5f;
                tile0_y = 0.0f;
                tile1_x = 0.5f;
                tile1_y = 0.0f;
                break;
            case 1:
                tile0_x = -0.5f;
                tile0_y = 0.5f;
                tile1_x = -0.5f;
                tile1_y = -0.5f;
                break;
            case 2:
                tile0_x = 0.5f;
                tile0_y = 0.0f;
                tile1_x = -0.5f;
                tile1_y = 0.0f;
                break;
            case 3:
                tile0_x = -0.5f;
                tile0_y = -0.5f;
                tile1_x = -0.5f;
                tile1_y = 0.5f;
                break;
        }

        return
        {
            Magnum::Vector2
            {
                -2.0f + tile0_x + x_offset,
                3.0f + tile0_y
            },
            Magnum::Vector2
            {
                -2.0f + tile1_x + x_offset,
                3.0f + tile1_y
            }
        };
    }
}

tile_grid::tile_grid(SceneGraph::DrawableGroup2D& drawables, Object2D* parent):
    Object2D{parent},
    drawables_(drawables)
{
//    for(int column_index = 0; column_index < 6; ++column_index)
//    {
//        for(int row_index = 0; row_index < 7; ++row_index)
//        {
//            auto& some_tile = addChild<tile>(drawables);
//            some_tile.translate({-2.5f + column_index, -5.0f + row_index});
//            some_tile.set_value(11 - (column_index + row_index));
//        }
//    }
//
//    auto& input_tile_0 = addChild<tile>(drawables);
//    input_tile_0.translate({-1.5f, 2.5f});
//    input_tile_0.set_value(0);
//
//    auto& input_tile_1 = addChild<tile>(drawables);
//    input_tile_1.translate({-1.5f, 3.5f});
//    input_tile_1.set_value(1);
//
//    auto& next_tile_0 = addChild<tile>(drawables);
//    next_tile_0.translate({-0.5f, 5.0f});
//    next_tile_0.set_value(2);
//
//    auto& next_tile_1 = addChild<tile>(drawables);
//    next_tile_1.translate({0.5f, 5.0f});
//    next_tile_1.set_value(3);
}

void tile_grid::create_next_input(const unsigned int value0, const unsigned int value1)
{
    next_input_tiles_[0] = &addChild<tile>(drawables_);
    next_input_tiles_[0]->translate({-0.5f, 5.0f});
    next_input_tiles_[0]->set_value(value0);

    next_input_tiles_[1] = &addChild<tile>(drawables_);
    next_input_tiles_[1]->translate({0.5f, 5.0f});
    next_input_tiles_[1]->set_value(value1);
}

void tile_grid::insert_next_input(const unsigned int x_offset, const unsigned int rotation)
{
    input_tiles_[0] = next_input_tiles_[0];
    input_tiles_[1] = next_input_tiles_[1];
    input_x_offset_ = x_offset;
    input_rotation_ = rotation;
    update_input_tiles_positions();
}

void tile_grid::set_input_x_offset(const unsigned int value)
{
    if(input_x_offset_ != value)
    {
        input_x_offset_ = value;
        update_input_tiles_positions();
    }
}

void tile_grid::set_input_rotation(const unsigned int value)
{
    if(input_rotation_ != value)
    {
        input_rotation_ = value;
        update_input_tiles_positions();
    }
}

void tile_grid::insert_input
(
    const unsigned int tile0_dst_column_index,
    const unsigned int tile0_dst_row_index,
    const unsigned int tile1_dst_column_index,
    const unsigned int tile1_dst_row_index
)
{
    if(input_tiles_[0])
    {
        board_tiles_[tile0_dst_column_index][tile0_dst_row_index] = input_tiles_[0];
        input_tiles_[0] = nullptr;
    }

    if(input_tiles_[1])
    {
        board_tiles_[tile1_dst_column_index][tile1_dst_row_index] = input_tiles_[1];
        input_tiles_[1] = nullptr;
    }
}

void tile_grid::drop_tiles(const data_types::tile_drop_list& drops)
{
    for(const auto& drop: drops)
    {
        if(const auto ptile = board_tiles_[drop.column_index][drop.src_row_index])
        {
            const auto dst_position = tile_coordinate_to_position(data_types::tile_coordinate{drop.column_index, drop.dst_row_index});
            board_tiles_[drop.column_index][drop.dst_row_index] = ptile;
            ptile->resetTransformation();
            ptile->translate(dst_position);
        }
    }
}

void tile_grid::update_input_tiles_positions()
{
    const auto dst_positions = get_input_tile_positions(input_x_offset_, input_rotation_);

    for(auto i = 0; i < 2; ++i)
    {
        input_tiles_[i]->resetTransformation();
        input_tiles_[i]->translate(dst_positions[i]);
    }
}

} //namespace
