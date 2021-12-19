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

#include <libgame/board.hpp>
#include <libutil/overload.hpp>

namespace libgame
{

int get_tile_count(const board& brd)
{
    auto count = 0;
    for(const auto& opt_tile: brd.tiles)
    {
        if(opt_tile)
        {
            ++count;
        }
    }
    return count;
}

std::optional<int> get_lowest_empty_cell
(
    const board& brd,
    const int col
)
{
    for(auto row = 0; row < brd.tiles.rows; ++row)
    {
        if(!at(brd.tiles, col, row))
        {
            return row;
        }
    }

    return std::nullopt;
}

bool is_overflowed(const board& brd)
{
    for(int col = 0; col < brd.tiles.cols; ++col)
    {
        if(at(brd.tiles, col, constants::board_authorized_row_count))
        {
            return true;
        }
    }

    return false;
}

int get_highest_tile_value(const board& brd)
{
    auto value = 0;
    for(const auto& opt_tile: brd.tiles)
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

int get_score(const board& brd)
{
    auto score = 0;
    for(const auto& opt_tile: brd.tiles)
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

apply_gravity_on_input_result apply_gravity_on_input
(
    const board& brd,
    const data_types::input_tile_matrix& input_tiles,
    const data_types::input_layout& input_layout
)
{
    apply_gravity_on_input_result result;
    result.brd = brd;

    //Make tiles fall from lowest to highest row of laid out input.
    for(auto input_row = 0; input_row < 2; ++input_row)
    {
        libutil::for_each_colrow
        (
            [&](auto& opt_tile, const int col, const int row)
            {
                if(!opt_tile)
                {
                    return;
                }

                const auto coord = get_tile_coordinate(input_layout, {col, row});

                //Process lower rows first, then higher ones
                if(coord.row != input_row)
                {
                    return;
                }

                const auto opt_dst_row = get_lowest_empty_cell(result.brd, coord.col);

                if(!opt_dst_row)
                {
                    return;
                }

                at(result.brd.tiles, coord.col, *opt_dst_row) = opt_tile;

                result.drops.push_back
                (
                    data_types::input_tile_drop
                    {
                        {col, row},
                        {coord.col, *opt_dst_row}
                    }
                );
            },
            input_tiles
        );
    }

    return result;
}

apply_gravity_result apply_gravity(const board& brd)
{
    auto result = apply_gravity_result{};
    result.brd = brd;

    for(int col = 0; col < result.brd.tiles.cols; ++col)
    {
        std::optional<int> opt_empty_cell_row;
        for(int row = 0; row < result.brd.tiles.rows; ++row) //from bottom to top
        {
            if(const auto& opt_tile = at(result.brd.tiles, col, row))
            {
                if(opt_empty_cell_row) //if the tile is floating
                {
                    at(result.brd.tiles, col, *opt_empty_cell_row) = opt_tile;
                    at(result.brd.tiles, col, row) = std::nullopt;

                    result.drops.push_back
                    (
                        data_types::board_tile_drop
                        {
                            col,
                            row,
                            *opt_empty_cell_row
                        }
                    );

                    ++*opt_empty_cell_row;
                }
            }
            else
            {
                if(!opt_empty_cell_row)
                {
                    opt_empty_cell_row = row;
                }
            }
        }
    }

    return result;
}

board apply_nullifiers
(
    board brd,
    libutil::matrix_coordinate_list& nullified_tiles_coords //output
)
{
    libutil::for_each_colrow
    (
        [&](auto& opt_tile, const int col, const int row)
        {
            if(!opt_tile)
            {
                return;
            }
            const auto& tile = *opt_tile;

            std::visit
            (
                libutil::overload
                {
                    [&](const data_types::tiles::number&){},

                    [&](const data_types::tiles::granite&){},

                    [&](const data_types::tiles::column_nullifier&)
                    {
                        //Remove all tiles from current column
                        for(int nullified_row = 0; nullified_row < brd.tiles.rows; ++nullified_row)
                        {
                            auto& opt_tile = at(brd.tiles, col, nullified_row);

                            if(!opt_tile)
                            {
                                continue;
                            }

                            nullified_tiles_coords.push_back({col, nullified_row});

                            opt_tile = std::nullopt;
                        }
                    },

                    [&](const data_types::tiles::row_nullifier&)
                    {
                        //Remove all tiles from current row
                        for(int nullified_col = 0; nullified_col < brd.tiles.cols; ++nullified_col)
                        {
                            auto& opt_tile = at(brd.tiles, nullified_col, row);

                            if(!opt_tile)
                            {
                                continue;
                            }

                            nullified_tiles_coords.push_back({nullified_col, row});

                            opt_tile = std::nullopt;
                        }
                    },

                    [&](const data_types::tiles::number_nullifier&)
                    {
                        //Remove the nullifier tile itself
                        opt_tile = std::nullopt;
                        nullified_tiles_coords.push_back({col, row});

                        //Get the value of the number tile placed below the
                        //nullifier tile, if any
                        const auto opt_value = [&]() -> std::optional<int>
                        {
                            if(row == 0)
                            {
                                return std::nullopt;
                            }

                            const auto& opt_below_tile = at(brd.tiles, col, row - 1);

                            if(!opt_below_tile)
                            {
                                return std::nullopt;
                            }

                            const auto pbelow_tile = std::get_if<data_types::tiles::number>(&*opt_below_tile);

                            if(!pbelow_tile)
                            {
                                return std::nullopt;
                            }

                            return pbelow_tile->value;
                        }();

                        if(!opt_value)
                        {
                            return;
                        }

                        //Remove all number tiles of that value
                        libutil::for_each_colrow
                        (
                            [&](auto& opt_tile, const int col, const int row)
                            {
                                if(!opt_tile)
                                {
                                    return;
                                }

                                const auto ptile = std::get_if<data_types::tiles::number>(&*opt_tile);

                                if(!ptile)
                                {
                                    return;
                                }

                                if(ptile->value != *opt_value)
                                {
                                    return;
                                }

                                opt_tile = std::nullopt;
                                nullified_tiles_coords.push_back({col, row});
                            },
                            brd.tiles
                        );
                    },

                    [&](const data_types::tiles::adder& tile){}
                },
                tile
            );
        },
        brd.tiles
    );

    return brd;
}

board apply_adders
(
    board brd,
    event_list& events
)
{
    libutil::for_each_colrow
    (
        [&](auto& opt_tile, const int col, const int row)
        {
            if(!opt_tile)
                return;

            const auto& tile = *opt_tile;

            const auto padder_tile = std::get_if<data_types::tiles::adder>(&tile);

            if(!padder_tile)
                return;

            auto event = events::tile_value_change{};
            event.nullified_tile_coordinate = {col, row};

            const auto adder_tile = *padder_tile;
            const auto adder_tile_value = adder_tile.value;

            //Remove the adder tile itself
            opt_tile = std::nullopt;

            //Get the value of the number tile placed below the
            //adder tile, if any
            const auto opt_value = [&]() -> std::optional<int>
            {
                if(row == 0)
                {
                    return std::nullopt;
                }

                const auto& opt_below_tile = at(brd.tiles, col, row - 1);

                if(!opt_below_tile)
                {
                    return std::nullopt;
                }

                const auto pbelow_tile = std::get_if<data_types::tiles::number>(&*opt_below_tile);

                if(!pbelow_tile)
                {
                    return std::nullopt;
                }

                return pbelow_tile->value;
            }();

            if(opt_value)
            {
                const auto current_value = *opt_value;

                const auto new_value = [&]
                {
                    //For positive adders
                    if(adder_tile_value > 0)
                    {
                        //Don't alter 9+ tiles
                        if(current_value >= 9)
                            return current_value;

                        //Cap new value to 9
                        return std::min(current_value + adder_tile_value, 9);
                    }

                    //Don't go below 0
                    return std::max(current_value + adder_tile_value, 0);
                }();

                //Alter value of all number tiles of that value
                libutil::for_each_colrow
                (
                    [&](auto& opt_tile, const int col, const int row)
                    {
                        if(!opt_tile)
                            return;

                        const auto ptile = std::get_if<data_types::tiles::number>(&*opt_tile);

                        if(!ptile)
                            return;

                        auto& tile = *ptile;

                        if(tile.value != current_value)
                            return;

                        if(tile.value == new_value)
                            return;

                        tile.value = new_value;

                        auto change = data_types::tile_value_change{};
                        change.coordinate = {col, row};
                        change.new_value = new_value;
                        event.changes.push_back(change);
                    },
                    brd.tiles
                );
            }

            events.push_back(event);
        },
        brd.tiles
    );

    return brd;
}

apply_merges_on_granites_result apply_merges_on_granites
(
    const board& brd,
    const data_types::tile_merge_list& merges
)
{
    auto result = apply_merges_on_granites_result{};
    result.brd = brd;

    libutil::for_each_colrow
    (
        [&](auto& opt_tile, const int col, const int row)
        {
            if(!opt_tile)
            {
                return;
            }
            auto& tile = *opt_tile;

            if(const auto pgranite = std::get_if<data_types::tiles::granite>(&tile))
            {
                auto& granite = *pgranite;

                const auto must_erode = [&]
                {
                    for(const auto& merge: merges)
                    {
                        for(const auto& src_tile_coordinate: merge.src_tile_coordinates)
                        {
                            if
                            (
                                src_tile_coordinate == libutil::matrix_coordinate{col - 1, row} ||
                                src_tile_coordinate == libutil::matrix_coordinate{col + 1, row} ||
                                src_tile_coordinate == libutil::matrix_coordinate{col, row - 1} ||
                                src_tile_coordinate == libutil::matrix_coordinate{col, row + 1}
                            )
                            {
                                return true;
                            }
                        }
                    }

                    return false;
                }();

                if(must_erode)
                {
                    --granite.thickness;

                    if(granite.thickness <= 0)
                    {
                        opt_tile = std::nullopt;
                    }

                    result.granite_erosions.push_back
                    (
                        data_types::granite_erosion
                        {
                            .coordinate = {col, row},
                            .new_thickness = granite.thickness
                        }
                    );
                }
            }
        },
        result.brd.tiles
    );

    return result;
}

} //namespace
