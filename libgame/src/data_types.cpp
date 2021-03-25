/*
Copyright 2018 - 2021 Florian Goujeon

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

#include <libgame/data_types.hpp>
#include <libutil/streamable.hpp>

namespace libgame::data_types
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

    std::ostream& operator<<(std::ostream& l, const column_nullifier&)
    {
        return l << "column_nullifier{}";
    }

    std::ostream& operator<<(std::ostream& l, const row_nullifier&)
    {
        return l << "row_nullifier{}";
    }

    std::ostream& operator<<(std::ostream& l, const number_nullifier&)
    {
        return l << "number_nullifier{}";
    }

    std::ostream& operator<<(std::ostream& l, const granite& r)
    {
        l << "granite";
        l << "{";
        l << "thickness: " << r.thickness;
        l << "}";
        return l;
    }
}



/*
board_tile_matrix associated functions
*/

bool is_overflowed(const board_tile_matrix& tiles)
{
    for(int col = 0; col < tiles.cols; ++col)
    {
        if(at(tiles, col, constants::board_authorized_row_count))
        {
            return true;
        }
    }

    return false;
}

int get_highest_tile_value(const board_tile_matrix& tiles)
{
    auto value = 0;
    for(const auto& opt_tile: tiles)
    {
        if(opt_tile)
        {
            if(const auto pnum_tile = std::get_if<data_types::tiles::number>(&*opt_tile))
            {
                value = std::max(value, pnum_tile->value);
            }
        }
    }
    return value;
}

int get_score(const board_tile_matrix& tiles)
{
    auto score = 0;
    for(const auto& opt_tile: tiles)
    {
        if(opt_tile)
        {
            if(const auto pnum_tile = std::get_if<data_types::tiles::number>(&*opt_tile))
            {
                score += std::pow(3, pnum_tile->value);
            }
        }
    }
    return score;
}



/*
input_layout associated functions
*/

std::ostream& operator<<(std::ostream& l, const input_layout& r)
{
    l << "input_layout";
    l << "{";
    l << "col_offset: " << r.col_offset << ", ";
    l << "rotation: " << r.rotation;
    l << "}";
    return l;
}

libutil::matrix_coordinate get_tile_coordinate
(
    const input_layout& layout,
    const libutil::matrix_coordinate& tile_coord //coordinate of tile in input
)
{
    const auto hash = [](const int in_col, const int in_row, const int rot)
    {
        return
            ((in_col & 1) << 3) |
            ((in_row & 1) << 2) |
            (rot & 3)
        ;
    };

#define CASE(IN_COL, IN_ROW, ROT, OUT_COL, OUT_ROW) \
    case hash(IN_COL, IN_ROW, ROT): \
        return libutil::matrix_coordinate{OUT_COL + layout.col_offset, OUT_ROW};

    switch(hash(tile_coord.col, tile_coord.row, layout.rotation))
    {
        //--(in_col, in_row, rot, out_col, out_row )
        CASE(0,      0,      0,   0,       0       );
        CASE(0,      0,      1,   0,       1       );
        CASE(0,      0,      2,   1,       1       );
        CASE(0,      0,      3,   1,       0       );
        CASE(0,      1,      0,   0,       1       );
        CASE(0,      1,      1,   1,       1       );
        CASE(0,      1,      2,   1,       0       );
        CASE(0,      1,      3,   0,       0       );
        CASE(1,      0,      0,   1,       0       );
        CASE(1,      0,      1,   0,       0       );
        CASE(1,      0,      2,   0,       1       );
        CASE(1,      0,      3,   1,       1       );
        CASE(1,      1,      0,   1,       1       );
        CASE(1,      1,      1,   1,       0       );
        CASE(1,      1,      2,   0,       0       );
        CASE(1,      1,      3,   0,       1       );
    }

#undef CASE

    assert(false);
    return libutil::matrix_coordinate{0, 0};
}



std::ostream& operator<<(std::ostream& l, const input_tile_drop& r)
{
    l << "input_tile_drop";
    l << "{";
    l << "input_coordinate: " << libutil::streamable{r.input_coordinate} << ", ";
    l << "board_coordinate: " << libutil::streamable{r.board_coordinate};
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
    l << "dst_tile_coordinate: " << libutil::streamable{r.dst_tile_coordinate} << ", ";
    l << "dst_tile_value: " << r.dst_tile_value;
    l << "}";
    return l;
}



std::ostream& operator<<(std::ostream& l, const granite_erosion& r)
{
    l << "granite_erosion";
    l << "{";
    l << "coordinate: " << libutil::streamable{r.coordinate} << ", ";
    l << "new_thickness: " << r.new_thickness;
    l << "}";
    return l;
}

} //namespace
