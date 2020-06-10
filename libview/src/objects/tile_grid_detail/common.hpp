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

#ifndef LIBVIEW_OBJECTS_TILE_GRID_DETAIL_COMMON_HPP
#define LIBVIEW_OBJECTS_TILE_GRID_DETAIL_COMMON_HPP

#include "../../animation.hpp"
#include "../../common.hpp"
#include <libview/data_types.hpp>

namespace libview::objects::tile_grid_detail
{

using input_tile_object_matrix = libutil::matrix
<
    std::shared_ptr<Object2D>,
    libcommon::constants::input_column_count,
    libcommon::constants::input_row_count
>;

std::shared_ptr<Object2D> make_tile_object
(
    Object2D& parent,
    features::drawable_group& drawables,
    features::animable_group& animables,
    const data_types::tile& tile
);

} //namespace

#endif
