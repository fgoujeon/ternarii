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

#include <libgame/board_functions.hpp>
#include <libgame/constants.hpp>
#include <libutil/overload.hpp>

namespace libgame::data_types
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
            if(const auto pnum_tile = std::get_if<tiles::number>(&*opt_tile))
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
            if(const auto pnum_tile = std::get_if<tiles::number>(&*opt_tile))
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
    const input_tile_matrix& input_tiles,
    const input_layout& input_layout
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
                    input_tile_drop
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
                        board_tile_drop
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

apply_nullifiers_result apply_nullifiers(const board& brd)
{
    auto result = apply_nullifiers_result{};
    result.brd = brd;

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
                    [&](const tiles::number&){},

                    [&](const tiles::granite&){},

                    [&](const tiles::column_nullifier&)
                    {
                        //Remove all tiles from current column
                        for(int nullified_row = 0; nullified_row < result.brd.tiles.rows; ++nullified_row)
                        {
                            auto& opt_tile = at(result.brd.tiles, col, nullified_row);

                            if(!opt_tile)
                            {
                                continue;
                            }

                            result.nullified_tiles_coords.push_back({col, nullified_row});

                            opt_tile = std::nullopt;
                        }
                    },

                    [&](const tiles::row_nullifier&)
                    {
                        //Remove all tiles from current row
                        for(int nullified_col = 0; nullified_col < result.brd.tiles.cols; ++nullified_col)
                        {
                            auto& opt_tile = at(result.brd.tiles, nullified_col, row);

                            if(!opt_tile)
                            {
                                continue;
                            }

                            result.nullified_tiles_coords.push_back({nullified_col, row});

                            opt_tile = std::nullopt;
                        }
                    },

                    [&](const tiles::number_nullifier&)
                    {
                        //Remove the nullifier tile itself
                        opt_tile = std::nullopt;
                        result.nullified_tiles_coords.push_back({col, row});

                        //Get the value of the number tile placed below the
                        //nullifier tile, if any
                        const auto opt_value = [&]() -> std::optional<int>
                        {
                            if(row == 0)
                            {
                                return std::nullopt;
                            }

                            const auto& opt_below_tile = at(result.brd.tiles, col, row - 1);

                            if(!opt_below_tile)
                            {
                                return std::nullopt;
                            }

                            const auto pbelow_tile = std::get_if<tiles::number>(&*opt_below_tile);

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

                                const auto ptile = std::get_if<tiles::number>(&*opt_tile);

                                if(!ptile)
                                {
                                    return;
                                }

                                if(ptile->value != *opt_value)
                                {
                                    return;
                                }

                                opt_tile = std::nullopt;
                                result.nullified_tiles_coords.push_back({col, row});
                            },
                            result.brd.tiles
                        );
                    },

                    [&](const tiles::adder& tile){}
                },
                tile
            );
        },
        result.brd.tiles
    );

    return result;
}

apply_adders_result apply_adders(const board& brd)
{
    auto result = apply_adders_result{};
    result.brd = brd;

    libutil::for_each_colrow
    (
        [&](auto& opt_tile, const int col, const int row)
        {
            if(!opt_tile)
                return;

            const auto& tile = *opt_tile;

            const auto padder_tile = std::get_if<tiles::adder>(&tile);

            if(!padder_tile)
                return;

            auto application = adder_tile_application{};
            application.nullified_tile_coordinate = {col, row};

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

                const auto& opt_below_tile = at(result.brd.tiles, col, row - 1);

                if(!opt_below_tile)
                {
                    return std::nullopt;
                }

                const auto pbelow_tile = std::get_if<tiles::number>(&*opt_below_tile);

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
                    //Don't alter 9+ tiles
                    if(current_value >= 9)
                        return current_value;

                    //Stay between 0 and 9
                    return std::clamp(current_value + adder_tile_value, 0, 9);
                }();

                const auto value_diff = new_value - current_value;

                //Alter value of all number tiles of that value
                if(value_diff != 0)
                {
                    libutil::for_each_colrow
                    (
                        [&](auto& opt_tile, const int col, const int row)
                        {
                            if(!opt_tile)
                                return;

                            const auto ptile = std::get_if<tiles::number>(&*opt_tile);

                            if(!ptile)
                                return;

                            auto& tile = *ptile;

                            if(tile.value != current_value)
                                return;

                            tile.value = new_value;

                            auto change = tile_value_change{};
                            change.coordinate = {col, row};
                            change.new_value = new_value;
                            change.value_diff = value_diff;
                            application.changes.push_back(change);
                        },
                        result.brd.tiles
                    );
                }
            }

            result.applications.push_back(application);
        },
        result.brd.tiles
    );

    return result;
}



namespace
{
    enum class selection_state
    {
        unselected,
        visited,
        selected
    };

    using selection_t = libutil::matrix
    <
        selection_state,
        constants::board_column_count,
        constants::board_row_count
    >;

    void select_tiles
    (
        const board& brd,
        const int tile_value,
        const int col,
        const int row,
        selection_t& selection,
        int& selection_size
    )
    {
        constexpr auto total_column_count = constants::board_column_count;
        constexpr auto total_row_count = constants::board_row_count;

        at(selection, col, row) = selection_state::visited;

        const auto& opt_tile = at(brd.tiles, col, row);
        if(!opt_tile)
        {
            return;
        }

        const auto pnum_tile = std::get_if<tiles::number>(&*opt_tile);
        if(!pnum_tile)
        {
            return;
        }

        const auto& tile = *pnum_tile;

        if(tile.value != tile_value)
        {
            return;
        }

        at(selection, col, row) = selection_state::selected;
        ++selection_size;

        //above tile
        if
        (
            row + 1 < total_row_count &&
            at(selection, col, row + 1) == selection_state::unselected
        )
        {
            select_tiles(brd, tile_value, col, row + 1, selection, selection_size);
        }

        //beneath tile
        if
        (
            row >= 1 &&
            at(selection, col, row - 1) == selection_state::unselected
        )
        {
            select_tiles(brd, tile_value, col, row - 1, selection, selection_size);
        }

        //right tile
        if
        (
            col + 1 < total_column_count &&
            at(selection, col + 1, row) == selection_state::unselected
        )
        {
            select_tiles(brd, tile_value, col + 1, row, selection, selection_size);
        }

        //left tile
        if
        (
            col >= 1 &&
            at(selection, col - 1, row) == selection_state::unselected
        )
        {
            select_tiles(brd, tile_value, col - 1, row, selection, selection_size);
        }
    }
}

apply_merges_result apply_merges(const board& brd)
{
    auto result = apply_merges_result{};
    auto& merges = result.merges;

    result.brd = brd;

    auto tile_layer = board_tile_matrix{};

    //Select the identical adjacent tiles.
    //Scan row by row, from the bottom left corner to the top right corner.
    for(int row = 0; row < result.brd.tiles.rows; ++row)
    {
        for(int col = 0; col < result.brd.tiles.cols; ++col)
        {
            const auto& opt_tile = at(result.brd.tiles, col, row);
            if(!opt_tile)
            {
                continue;
            }

            const auto pnum_tile = std::get_if<tiles::number>(&*opt_tile);
            if(!pnum_tile)
            {
                continue;
            }

            const auto& current_tile = *pnum_tile;

            auto selection = selection_t{}; //fill with unselected
            auto selection_size = 0;
            select_tiles
            (
                result.brd,
                current_tile.value,
                col,
                row,
                selection,
                selection_size
            );

            //if 3 or more tiles are selected
            if(selection_size >= 3)
            {
                //remove the selected tiles from the board
                libutil::matrix_coordinate_list removed_tile_coordinates;
                libutil::for_each_colrow
                (
                    [&](auto& opt_tile2, const int col2, const int row2)
                    {
                        if(at(selection, col2, row2) == selection_state::selected)
                        {
                            assert(opt_tile2.has_value());
                            removed_tile_coordinates.push_back({col2, row2});
                            opt_tile2 = std::nullopt;
                        }
                    },
                    result.brd.tiles
                );

                //put the new merged tile on the layer
                auto merged_tile = tiles::number{current_tile.value + 1};
                at(tile_layer, col, row) = merged_tile;

                merges.push_back
                (
                    tile_merge
                    {
                        removed_tile_coordinates,
                        libutil::matrix_coordinate{col, row},
                        merged_tile.value
                    }
                );
            }
        }
    }

    if(!merges.empty())
    {
        //overlay the tile layer to the tile array of the board
        libutil::for_each
        (
            [&](auto& opt_tile, const auto& opt_layer_tile)
            {
                if(opt_layer_tile)
                {
                    assert(!opt_tile);
                    opt_tile = opt_layer_tile;
                }
            },
            result.brd.tiles,
            tile_layer
        );
    }

    return result;
}



apply_merges_on_granites_result apply_merges_on_granites
(
    const board& brd,
    const tile_merge_list& merges
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

            if(const auto pgranite = std::get_if<tiles::granite>(&tile))
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
                        granite_erosion
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



drop_input_tiles_result drop_input_tiles
(
    const board& brd0,
    const input_tile_matrix& input_tiles,
    const input_layout& input_layout
)
{
    auto final_result = drop_input_tiles_result{};
    auto& brd = final_result.brd;
    auto& events = final_result.events;

    brd = brd0;

    //Apply gravity on input tiles
    {
        auto result = apply_gravity_on_input
        (
            brd,
            input_tiles,
            input_layout
        );
        brd = result.brd;
        events.push_back(events::input_tile_drop{std::move(result.drops)});
    }

    auto old_event_count = 0;
    do
    {
        old_event_count = events.size();

        //Apply nullifier tiles
        {
            auto result = apply_nullifiers(brd);
            brd = std::move(result.brd);
            if(!result.nullified_tiles_coords.empty())
            {
                events.push_back
                (
                    events::tile_nullification
                    {
                        std::move(result.nullified_tiles_coords)
                    }
                );
            }
        }

        //Apply adders
        {
            auto result = apply_adders(brd);
            brd = std::move(result.brd);
            for(const auto& application: result.applications)
            {
                events.push_back
                (
                    events::tile_value_change
                    {
                        application.nullified_tile_coordinate,
                        application.changes
                    }
                );
            }
        }

        //Merge number tiles
        const auto merges = [&]
        {
            auto result = apply_merges(brd);
            brd = std::move(result.brd);
            return result.merges;
        }();

        //Decrease thickness of granite tiles
        if(!merges.empty())
        {
            const auto result = apply_merges_on_granites(brd, merges);

            brd = result.brd;

            events.push_back
            (
                events::tile_merge
                {
                    .merges = merges,
                    .granite_erosions = result.granite_erosions
                }
            );
        }

        //Apply gravity
        {
            auto result = apply_gravity(brd);
            brd = std::move(result.brd);
            if(!result.drops.empty())
                events.push_back(events::board_tile_drop{std::move(result.drops)});
        }
    } while(old_event_count != events.size());

    events.push_back(events::score_change{get_score(brd)});

    return final_result;
}

} //namespace
