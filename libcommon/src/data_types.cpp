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

#include <libcommon/data_types.hpp>
#include <libutil/streamable.hpp>

namespace libcommon::data_types
{

std::ostream& operator<<(std::ostream& l, const tile& r)
{
    l << "tile";
    l << "{";
    l << "value: " << r.value;
    l << "}";
    return l;
}



std::ostream& operator<<(std::ostream& l, const input_layout& r)
{
    l << "input_layout";
    l << "{";
    l << "x_offset: " << r.x_offset << ", ";
    l << "rotation: " << r.rotation;
    l << "}";
    return l;
}

tile_coordinate get_tile_coordinate
(
    const input_layout& layout,
    const int tile_index //index of tile in input
)
{
    if(tile_index == 0)
    {
        const int x = layout.x_offset + (layout.rotation == 2 ? 1 : 0);
        const int y = (layout.rotation == 1 ? 1 : 0);
        return {x, y};
    }
    else
    {
        const int x = layout.x_offset + (layout.rotation == 0 ? 1 : 0);
        const int y = (layout.rotation == 3 ? 1 : 0);
        return {x, y};
    }
}



std::ostream& operator<<(std::ostream& l, const tile_coordinate& r)
{
    l << "tile_coordinate";
    l << "{";
    l << "column_index: " << r.column_index << ", ";
    l << "row_index: " << r.row_index;
    l << "}";
    return l;
}



std::ostream& operator<<(std::ostream& l, const tile_drop& r)
{
    l << "tile_drop";
    l << "{";
    l << "column_index: " << r.column_index << ", ";
    l << "src_row_index: " << r.src_row_index << ", ";
    l << "dst_row_index: " << r.dst_row_index;
    l << "}";
    return l;
}



std::ostream& operator<<(std::ostream& l, const tile_merge& r)
{
    l << "tile_merge";
    l << "{";
    l << "src_tile_coordinates: " << libutil::streamable{r.src_tile_coordinates} << ", ";
    l << "dst_tile_coordinate: " << r.dst_tile_coordinate << ", ";
    l << "dst_tile_value: " << r.dst_tile_value;
    l << "}";
    return l;
}

} //namespace
