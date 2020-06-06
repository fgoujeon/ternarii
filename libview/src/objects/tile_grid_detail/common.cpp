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

#include "common.hpp"
#include "../number_tile.hpp"
#include "../sdf_image_tile.hpp"
#include <libres.hpp>
#include <libutil/overload.hpp>

namespace libview::objects::tile_grid_detail
{

std::shared_ptr<Object2D> make_tile_object
(
    Object2D& parent,
    features::drawable_group& drawables,
    const data_types::tile& tile
)
{
    using result_t = std::shared_ptr<Object2D>;

    auto ptile = std::visit
    (
        libutil::overload
        {
            [&](const data_types::number_tile& tile) -> result_t
            {
                return std::make_shared<number_tile>(parent, drawables, tile.value);
            },
            [&](const data_types::column_nullifier_tile&) -> result_t
            {
                return std::make_shared<sdf_image_tile>(parent, drawables, libres::images::column_nullifier);
            },
            [&](const data_types::row_nullifier_tile&) -> result_t
            {
                return std::make_shared<sdf_image_tile>(parent, drawables, libres::images::row_nullifier);
            },
            [&](const data_types::number_nullifier_tile&) -> result_t
            {
                return std::make_shared<sdf_image_tile>(parent, drawables, libres::images::number_nullifier);
            }
        },
        tile
    );

    ptile->set_alpha(0.0f);
    ptile->scale({0.48f, 0.48f});

    return ptile;
}

} //namespace
