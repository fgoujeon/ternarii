#ifndef BOARD_GRID_HPP
#define BOARD_GRID_HPP

#include "item_grid.hpp"
#include <array>
#include <memory>

namespace libgame
{

using board_grid_t = item_grid_t<6, 10>; //cell = grid[column_index][row_index]

} //namespace libgame

#endif
