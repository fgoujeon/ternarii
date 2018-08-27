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

struct element
{
    explicit element(unsigned int value): value(value)
    {
    }

    unsigned int value;
};

typedef std::optional<element> item_grid_cell_t;

template<size_t ColumnCount, size_t RowCount>
using item_grid_t = array2d<item_grid_cell_t, ColumnCount, RowCount>;

using board_grid_t = item_grid_t<6, 10>; //cell = grid[column_index][row_index]

typedef std::array<element, 2> board_next_input_t;

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
