#ifndef LIBGAME_DATA_TYPES_HPP
#define LIBGAME_DATA_TYPES_HPP

#include <optional>
#include <array>
#include <memory>
#include <ostream>

namespace libgame
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

} //namespace libgame

#endif
