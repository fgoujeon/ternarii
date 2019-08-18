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

} //namespace
