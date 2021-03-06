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

#include "board.hpp"
#include <libgame/data_types.hpp>
#include <libutil/overload.hpp>
#include <memory>
#include <cmath>
#include <cassert>

namespace libgame
{

namespace
{
    /*
    Return:
    - board tile matrix after input tiles are dropped
    - list of tile drops
    */
    data_types::board_tile_matrix apply_gravity
    (
        const data_types::input_tile_matrix& input_tiles,
        const data_types::input_layout& input_layout,
        data_types::board_tile_matrix board_tiles,
        data_types::input_tile_drop_list* pdrops //output, optional
    )
    {
        auto drops = data_types::input_tile_drop_list{};

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

                    const auto opt_dst_row = data_types::get_lowest_empty_cell
                    (
                        board_tiles,
                        coord.col
                    );

                    if(!opt_dst_row)
                    {
                        return;
                    }

                    at(board_tiles, coord.col, *opt_dst_row) = opt_tile;

                    if(pdrops)
                    {
                        pdrops->push_back
                        (
                            data_types::input_tile_drop
                            {
                                {col, row},
                                {coord.col, *opt_dst_row}
                            }
                        );
                    }
                },
                input_tiles
            );
        }

        return board_tiles;
    }

    /*
    Return:
    - board tile matrix after application of nullifiers
    - coordinates of nullified tiles
    */
    data_types::board_tile_matrix apply_nullifiers
    (
        data_types::board_tile_matrix tiles,
        libutil::matrix_coordinate_list& coords //output
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
                            for(int nullified_row = 0; nullified_row < tiles.rows; ++nullified_row)
                            {
                                auto& opt_tile = at(tiles, col, nullified_row);

                                if(!opt_tile)
                                {
                                    continue;
                                }

                                coords.push_back({col, nullified_row});

                                opt_tile = std::nullopt;
                            }
                        },

                        [&](const data_types::tiles::row_nullifier&)
                        {
                            //Remove all tiles from current row
                            for(int nullified_col = 0; nullified_col < tiles.cols; ++nullified_col)
                            {
                                auto& opt_tile = at(tiles, nullified_col, row);

                                if(!opt_tile)
                                {
                                    continue;
                                }

                                coords.push_back({nullified_col, row});

                                opt_tile = std::nullopt;
                            }
                        },

                        [&](const data_types::tiles::number_nullifier&)
                        {
                            //Remove the nullifier tile itself
                            opt_tile = std::nullopt;
                            coords.push_back({col, row});

                            //Get the value of the number tile placed below the
                            //nullifier tile, if any
                            const auto opt_value = [&]() -> std::optional<int>
                            {
                                if(row == 0)
                                {
                                    return std::nullopt;
                                }

                                const auto& opt_below_tile = at(tiles, col, row - 1);

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
                                    coords.push_back({col, row});
                                },
                                tiles
                            );
                        }
                    },
                    tile
                );
            },
            tiles
        );

        return tiles;
    }

    struct apply_merges_on_granites_result
    {
        data_types::board_tile_matrix tiles;
        data_types::granite_erosion_list granite_erosions;
    };

    /*
    Decrement the thickness of granites whose adjacent tiles contain a number
    tile involved in a merge.
    */
    apply_merges_on_granites_result apply_merges_on_granites
    (
        const data_types::board_tile_matrix& tiles,
        const data_types::tile_merge_list& merges
    )
    {
        auto result = apply_merges_on_granites_result{};
        result.tiles = tiles;

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
            result.tiles
        );

        return result;
    }
}

board::board(data_types::board_tile_matrix& tiles):
    tiles_(tiles)
{
}

void board::get_targeted_tiles
(
    const data_types::input_tile_matrix& input_tiles,
    const data_types::input_layout& input_layout,
    libutil::matrix_coordinate_list& coords
) const
{
    const auto board_tiles = apply_gravity(input_tiles, input_layout, tiles_, nullptr);
    apply_nullifiers(board_tiles, coords);
}

void board::drop_input_tiles
(
    const data_types::input_tile_matrix& input_tiles,
    const data_types::input_layout& input_layout,
    event_list& events
)
{
    //Apply gravity on input tiles
    {
        auto drops = data_types::input_tile_drop_list{};
        tiles_ = apply_gravity(input_tiles, input_layout, tiles_, &drops);
        events.push_back(events::input_tile_drop{std::move(drops)});
    }

    auto old_event_count = 0;
    do
    {
        old_event_count = events.size();

        //Apply nullifier tiles
        {
            auto nullified_tiles = libutil::matrix_coordinate_list{};
            tiles_ = apply_nullifiers(tiles_, nullified_tiles);
            if(!nullified_tiles.empty())
            {
                events.push_back(events::tile_nullification{std::move(nullified_tiles)});
            }
        }

        //Merge number tiles
        const auto merges = merge_tiles();

        //Decrease thickness of granite tiles
        if(!merges.empty())
        {
            const auto result = apply_merges_on_granites(tiles_, merges);

            tiles_ = result.tiles;

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
        const auto drops = make_tiles_fall();
        if(!drops.empty())
        {
            events.push_back(events::board_tile_drop{drops});
        }
    } while(old_event_count != events.size());

    events.push_back(events::score_change{data_types::get_score(tiles_)});
}

data_types::board_tile_drop_list board::make_tiles_fall()
{
    data_types::board_tile_drop_list drops;

    for(int col = 0; col < tiles_.cols; ++col)
    {
        std::optional<int> opt_empty_cell_row;
        for(int row = 0; row < tiles_.rows; ++row) //from bottom to top
        {
            if(const auto& opt_tile = at(tiles_, col, row))
            {
                if(opt_empty_cell_row) //if the tile is floating
                {
                    at(tiles_, col, *opt_empty_cell_row) = opt_tile;
                    at(tiles_, col, row) = std::nullopt;

                    drops.push_back
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

    return drops;
}

data_types::tile_merge_list board::merge_tiles()
{
    data_types::tile_merge_list merges;

    data_types::board_tile_matrix tile_layer;

    //Select the identical adjacent tiles.
    //Scan row by row, from the bottom left corner to the top right corner.
    for(int row = 0; row < tiles_.rows; ++row)
    {
        for(int col = 0; col < tiles_.cols; ++col)
        {
            const auto& opt_tile = at(tiles_, col, row);
            if(!opt_tile)
            {
                continue;
            }

            const auto pnum_tile = std::get_if<data_types::tiles::number>(&*opt_tile);
            if(!pnum_tile)
            {
                continue;
            }

            const auto& current_tile = *pnum_tile;

            auto selection = selection_t{}; //fill with unselected
            auto selection_size = 0;

            select_tiles(current_tile.value, col, row, selection, selection_size);

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
                    tiles_
                );

                //put the new merged tile on the layer
                auto merged_tile = data_types::tiles::number{current_tile.value + 1};
                at(tile_layer, col, row) = merged_tile;

                merges.push_back
                (
                    data_types::tile_merge
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
            tiles_,
            tile_layer
        );
    }

    return merges;
}

void board::select_tiles
(
    const int tile_value,
    const int col,
    const int row,
    selection_t& selection,
    int& selection_size
)
{
    at(selection, col, row) = selection_state::visited;

    const auto& opt_tile = at(tiles_, col, row);
    if(!opt_tile)
    {
        return;
    }

    const auto pnum_tile = std::get_if<data_types::tiles::number>(&*opt_tile);
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
        select_tiles(tile_value, col, row + 1, selection, selection_size);
    }

    //beneath tile
    if
    (
        row >= 1 &&
        at(selection, col, row - 1) == selection_state::unselected
    )
    {
        select_tiles(tile_value, col, row - 1, selection, selection_size);
    }

    //right tile
    if
    (
        col + 1 < total_column_count &&
        at(selection, col + 1, row) == selection_state::unselected
    )
    {
        select_tiles(tile_value, col + 1, row, selection, selection_size);
    }

    //left tile
    if
    (
        col >= 1 &&
        at(selection, col - 1, row) == selection_state::unselected
    )
    {
        select_tiles(tile_value, col - 1, row, selection, selection_size);
    }
}

} //namespace
