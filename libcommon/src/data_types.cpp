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

namespace tiles
{
    std::ostream& operator<<(std::ostream& l, const number& r)
    {
        l << "number";
        l << "{";
        l << "value: " << r.value;
        l << "}";
        return l;
    }

    std::ostream& operator<<(std::ostream& l, const column_nullifier& r)
    {
        return l << "column_nullifier{}";
    }

    std::ostream& operator<<(std::ostream& l, const row_nullifier& r)
    {
        return l << "row_nullifier{}";
    }

    std::ostream& operator<<(std::ostream& l, const number_nullifier& r)
    {
        return l << "number_nullifier{}";
    }
}



std::ostream& operator<<(std::ostream& l, const input_layout& r)
{
    l << "input_layout";
    l << "{";
    l << "col_offset: " << r.col_offset << ", ";
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
    const auto hash = [](const int in_row, const int in_col, const int rot)
    {
        return
            ((in_row & 1) << 3) |
            ((in_col & 1) << 2) |
            (rot & 3)
        ;
    };

#define CASE(IN_ROW, IN_COL, ROT, OUT_ROW, OUT_COL) \
    case hash(IN_ROW, IN_COL, ROT): \
        return tile_coordinate{OUT_ROW, OUT_COL + layout.col_offset};

    switch(hash(tile_coord.row, tile_coord.col, layout.rotation))
    {
        //--(in_row, in_col, rot, out_row, out_col)
        CASE(0,      0,      0,   0,       0);
        CASE(0,      0,      1,   1,       0);
        CASE(0,      0,      2,   1,       1);
        CASE(0,      0,      3,   0,       1);
        CASE(0,      1,      0,   0,       1);
        CASE(0,      1,      1,   0,       0);
        CASE(0,      1,      2,   1,       0);
        CASE(0,      1,      3,   1,       1);
        CASE(1,      0,      0,   1,       0);
        CASE(1,      0,      1,   1,       1);
        CASE(1,      0,      2,   0,       1);
        CASE(1,      0,      3,   0,       0);
        CASE(1,      1,      0,   1,       1);
        CASE(1,      1,      1,   0,       1);
        CASE(1,      1,      2,   0,       0);
        CASE(1,      1,      3,   1,       0);
    }

#undef CASE

    assert(false);
    return tile_coordinate{0, 0};
}



std::ostream& operator<<(std::ostream& l, const tile_coordinate& r)
{
    l << "tile_coordinate";
    l << "{";
    l << "row: " << r.row << ", ";
    l << "col: " << r.col;
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
    l << "col: " << r.col << ", ";
    l << "src_row: " << r.src_row << ", ";
    l << "dst_row: " << r.dst_row;
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
