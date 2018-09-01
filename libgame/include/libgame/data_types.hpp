/*
Copyright 2018 Florian Goujeon

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

#ifndef LIBGAME_DATA_TYPES_HPP
#define LIBGAME_DATA_TYPES_HPP

#include <optional>
#include <vector>
#include <array>
#include <memory>
#include <ostream>

namespace libgame { namespace data_types
{

template<typename T, size_t Size0, size_t Size1>
using array2d = std::array<std::array<T, Size1>, Size0>;

struct tile
{
    explicit tile(unsigned int value): value(value)
    {
    }

    unsigned int value;
};

using tile_pair = std::array<tile, 2>;

/*
Convention for accessing the tiles:
grid[column_index][row_index]

Convention of rows and columns:
[..]
[R4]
[R3]
[R2]
[R1]
[R0]
	[C0][C1][C2][C3][..]
*/
template<size_t ColumnCount, size_t RowCount>
using tile_grid = array2d<std::optional<tile>, ColumnCount, RowCount>;

using board_tile_grid = tile_grid<6, 10>;

struct tile_coordinate
{
    unsigned int x;
    unsigned int y;
};

inline
std::ostream& operator<<(std::ostream& l, const tile_coordinate& r)
{
    l << "tile_coordinate";
    l << "{";
    l << "x: " << r.x << ", ";
    l << "y: " << r.y;
    l << "}";
    return l;
}



struct tile_drop
{
    unsigned int column_index;
    unsigned int src_row_index;
    unsigned int dst_row_index;
};

inline
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



struct tile_merge
{
    std::vector<tile_coordinate> src_tile_coordinates;
    tile_coordinate dst_tile_coordinate;
    unsigned int dst_tile_value;
};

inline
std::ostream& operator<<(std::ostream& l, const tile_merge& r)
{
    l << "tile_merge";
    l << "{";
    l << "src_tile_coordinates: {";
    {
        auto first = true;
        for(const auto& coord: r.src_tile_coordinates)
        {
            if(!first) l << ", ";
            l << coord;
            first = false;
        }
    }
    l << "}, ";
    l << "dst_tile_coordinate: " << r.dst_tile_coordinate << ", ";
    l << "dst_tile_value: " << r.dst_tile_value;
    l << "}";
    return l;
}

}} //namespace libgame::data_types

#endif
