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

std::ostream& operator<<(std::ostream& l, const number_tile& r)
{
    l << "number_tile";
    l << "{";
    l << "value: " << r.value;
    l << "}";
    return l;
}



std::ostream& operator<<(std::ostream& l, const vertical_dynamite_tile& r)
{
    return l << "vertical_dynamite_tile{}";
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
    const tile_coordinate& tile_coord //coordinate of tile in input
)
{
    const auto hash = [](const int xin, const int yin, const int rot)
    {
        return
            ((xin & 1) << 3) |
            ((yin & 1) << 2) |
            (rot & 3)
        ;
    };

#define CASE(XIN, YIN, ROT, XOUT, YOUT) \
    case hash(XIN, YIN, ROT): \
        return tile_coordinate{XOUT + layout.x_offset, YOUT};

    switch(hash(tile_coord.column_index, tile_coord.row_index, layout.rotation))
    {
        //--(xin, yin, rot, xout, yout)
        CASE(0,   0,   0,   0,    0);
        CASE(0,   0,   1,   0,    1);
        CASE(0,   0,   2,   1,    1);
        CASE(0,   0,   3,   1,    0);
        CASE(1,   0,   0,   1,    0);
        CASE(1,   0,   1,   0,    0);
        CASE(1,   0,   2,   0,    1);
        CASE(1,   0,   3,   1,    1);
        CASE(0,   1,   0,   0,    1);
        CASE(0,   1,   1,   1,    1);
        CASE(0,   1,   2,   1,    0);
        CASE(0,   1,   3,   0,    0);
        CASE(1,   1,   0,   1,    1);
        CASE(1,   1,   1,   1,    0);
        CASE(1,   1,   2,   0,    0);
        CASE(1,   1,   3,   0,    1);
    }

#undef CASE

    return tile_coordinate{0, 0};
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



std::ostream& operator<<(std::ostream& l, const input_tile_drop& r)
{
    l << "input_tile_drop";
    l << "{";
    l << "input_coordinate: " << r.input_coordinate << ", ";
    l << "board_coordinate: " << r.board_coordinate;
    l << "}";
    return l;
}



std::ostream& operator<<(std::ostream& l, const board_tile_drop& r)
{
    l << "board_tile_drop";
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



std::ostream& operator<<(std::ostream& l, const vertical_dynamite_tile_explosion& r)
{
    l << "vertical_dynamite_tile_explosion";
    l << "{";
    l << "coordinate: " << r.coordinate;
    l << "}";
    return l;
}

} //namespace
