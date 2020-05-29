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

std::ostream& operator<<(std::ostream& l, const column_nullifier_tile& r)
{
    return l << "column_nullifier_tile{}";
}

std::ostream& operator<<(std::ostream& l, const row_nullifier_tile& r)
{
    return l << "row_nullifier_tile{}";
}

std::ostream& operator<<(std::ostream& l, const number_nullifier_tile& r)
{
    return l << "number_nullifier_tile{}";
}



/*
board_tile_matrix associated functions
*/

bool is_overflowed(const board_tile_matrix& tiles)
{
    for(int col = 0; col < tiles.n; ++col)
    {
        if(libutil::at(tiles, constants::board_authorized_row_count, col))
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
            if(const auto pnum_tile = std::get_if<data_types::number_tile>(&*opt_tile))
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
            if(const auto pnum_tile = std::get_if<data_types::number_tile>(&*opt_tile))
            {
                score += std::pow(3, pnum_tile->value);
            }
        }
    }
    return score;
}

int get_free_cell_count(const board_tile_matrix& tiles)
{
    auto count = constants::board_authorized_cell_count;
    for(const auto& opt_tile: tiles)
    {
        if(opt_tile)
        {
            --count;
        }
    }
    return count;
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
        return libutil::matrix_coordinate{OUT_ROW, OUT_COL + layout.col_offset};

    switch(hash(tile_coord.i, tile_coord.j, layout.rotation))
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



std::string_view get_pretty_name(const stage s)
{
    switch(s)
    {
        case stage::purity_room:    return "PURITY ROOM";
        case stage::nullifier_room: return "NULLIFIER ROOM";
    }
    return "";
}

} //namespace
