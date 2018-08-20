#ifndef ITEM_GRID_HPP
#define ITEM_GRID_HPP

#include "array2d.hpp"
#include "element.hpp"
#include <optional>
#include <memory>

namespace libgame
{

typedef std::optional<std::shared_ptr<element>> item_grid_cell_t;

template<size_t ColumnCount, size_t RowCount>
using item_grid_t = array2d<item_grid_cell_t, ColumnCount, RowCount>;

} //namespace libgame

#endif
