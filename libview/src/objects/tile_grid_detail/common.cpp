/*
Copyright 2018 - 2022 Florian Goujeon

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
#include "../adder_tile.hpp"
#include "../number_tile.hpp"
#include "../sdf_image_tile.hpp"
#include "../granite_tile.hpp"
#include <libres.hpp>
#include <libutil/overload.hpp>

namespace libview::objects::tile_grid_detail
{

std::shared_ptr<object2d> make_tile_object
(
    object2d& parent,
    features::drawable_group& drawables,
    features::animable_group& animables,
    const data_types::tile& tile
)
{
    using result_t = std::shared_ptr<object2d>;
    using path_list = std::vector<std::filesystem::path>;

    return std::visit
    (
        libutil::overload
        {
            [&](const data_types::tiles::number& tile) -> result_t
            {
                return std::make_shared<number_tile>
                (
                    parent,
                    drawables,
                    animables,
                    tile.value
                );
            },
            [&](const data_types::tiles::column_nullifier&) -> result_t
            {
                return std::make_shared<sdf_image_tile>
                (
                    parent,
                    drawables,
                    path_list
                    {
                        libres::images::special_tile_symbol_null,
                        libres::images::special_tile_modifier_column
                    }
                );
            },
            [&](const data_types::tiles::row_nullifier&) -> result_t
            {
                return std::make_shared<sdf_image_tile>
                (
                    parent,
                    drawables,
                    path_list
                    {
                        libres::images::special_tile_symbol_null,
                        libres::images::special_tile_modifier_row
                    }
                );
            },
            [&](const data_types::tiles::number_nullifier&) -> result_t
            {
                return std::make_shared<sdf_image_tile>
                (
                    parent,
                    drawables,
                    path_list
                    {
                        libres::images::special_tile_symbol_null,
                        libres::images::special_tile_modifier_star
                    }
                );
            },
            [&](const data_types::tiles::outer_columns_nullifier&) -> result_t
            {
                return std::make_shared<sdf_image_tile>
                (
                    parent,
                    drawables,
                    path_list
                    {
                        libres::images::special_tile_symbol_null,
                        libres::images::special_tile_modifier_outer_columns
                    }
                );
            },
            [&](const data_types::tiles::granite& tile) -> result_t
            {
                return std::make_shared<granite_tile>
                (
                    parent,
                    drawables,
                    tile.thickness
                );
            },
            [&](const data_types::tiles::adder& tile) -> result_t
            {
                return std::make_shared<adder_tile>
                (
                    parent,
                    drawables,
                    tile.value
                );
            }
        },
        tile
    );
}

std::shared_ptr<object2d> make_preview_tile_object
(
    object2d& parent,
    features::drawable_group& drawables,
    features::animable_group&,
    const data_types::tile& tile
)
{
    using result_t = std::shared_ptr<object2d>;
    using path_list = std::vector<std::filesystem::path>;

    return std::visit
    (
        libutil::overload
        {
            [&](const data_types::tiles::number&) -> result_t
            {
                return nullptr;
            },
            [&](const data_types::tiles::column_nullifier&) -> result_t
            {
                return std::make_shared<sdf_image_tile>
                (
                    parent,
                    drawables,
                    path_list
                    {
                        libres::images::special_tile_symbol_null
                    }
                );
            },
            [&](const data_types::tiles::row_nullifier&) -> result_t
            {
                return std::make_shared<sdf_image_tile>
                (
                    parent,
                    drawables,
                    path_list
                    {
                        libres::images::special_tile_symbol_null
                    }
                );
            },
            [&](const data_types::tiles::number_nullifier&) -> result_t
            {
                return std::make_shared<sdf_image_tile>
                (
                    parent,
                    drawables,
                    path_list
                    {
                        libres::images::special_tile_symbol_null
                    }
                );
            },
            [&](const data_types::tiles::outer_columns_nullifier&) -> result_t
            {
                return std::make_shared<sdf_image_tile>
                (
                    parent,
                    drawables,
                    path_list
                    {
                        libres::images::special_tile_symbol_null
                    }
                );
            },
            [&](const data_types::tiles::granite&) -> result_t
            {
                return nullptr;
            },
            [&](const data_types::tiles::adder& tile) -> result_t
            {
                return std::make_shared<adder_tile>
                (
                    parent,
                    drawables,
                    tile.value,
                    false
                );
            }
        },
        tile
    );
}

} //namespace
