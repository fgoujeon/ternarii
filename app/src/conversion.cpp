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

#include "conversion.hpp"

namespace conversion
{

libview::data_types::board_item_array to_view(const libgame::data_types::board_tile_grid& from)
{
    auto to = libview::data_types::board_item_array{};

    int column_index = 0;
    for(const auto& column_tiles: from)
    {
        int row_index = 0;
        for(const auto& opt_tile: column_tiles)
        {
            if(opt_tile.has_value())
            {
                to[column_index][row_index] = libview::data_types::item{opt_tile.value().value};
            }
            ++row_index;
        }
        ++column_index;
    }

    return to;
}

libview::data_types::tile_coordinate to_view(const libgame::data_types::tile_coordinate& from)
{
    auto to = libview::data_types::tile_coordinate{};
    to.x = from.x;
    to.y = from.y;
    return to;
}

libview::data_types::tile_drop to_view(const libgame::data_types::tile_drop& from)
{
    auto to = libview::data_types::tile_drop{};
    to.column_index = from.column_index;
    to.src_row_index = from.src_row_index;
    to.dst_row_index = from.dst_row_index;
    return to;
}

libview::data_types::tile_merge to_view(const libgame::data_types::tile_merge& from)
{
    auto to = libview::data_types::tile_merge{};
    to.src_tile_coordinates = to_view(from.src_tile_coordinates);
    to.dst_tile_coordinate = to_view(from.dst_tile_coordinate);
    to.dst_tile_value = from.dst_tile_value;
    return to;
}

} //namespace conversion
