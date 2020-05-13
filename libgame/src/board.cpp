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

#include "board.hpp"
#include "board_input.hpp"
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
    data_types::board_tile_array apply_gravity
    (
        const data_types::input_tile_array& input_tiles,
        const data_types::input_layout& input_layout,
        data_types::board_tile_array board_tiles,
        data_types::input_tile_drop_list* pdrops //output, optional
    )
    {
        auto drops = data_types::input_tile_drop_list{};

        //Make tiles fall from lowest to highest row of laid out input.
        for(auto input_row = 0; input_row < 2; ++input_row)
        {
            libutil::for_each_ij
            (
                [&](auto& opt_tile, const int row, const int col)
                {
                    if(!opt_tile)
                    {
                        return;
                    }

                    const auto coord = get_tile_coordinate(input_layout, {row, col});

                    //Process lower rows first, then higher ones
                    if(coord.row != input_row)
                    {
                        return;
                    }

                    const auto opt_dst_row = libcommon::data_types::get_lowest_empty_cell
                    (
                        board_tiles,
                        coord.col
                    );

                    if(!opt_dst_row)
                    {
                        return;
                    }

                    libutil::at(board_tiles, *opt_dst_row, coord.col) = opt_tile;

                    if(pdrops)
                    {
                        pdrops->push_back
                        (
                            data_types::input_tile_drop
                            {
                                {row, col},
                                {*opt_dst_row, coord.col}
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
    data_types::board_tile_array apply_nullifiers
    (
        data_types::board_tile_array tiles,
        data_types::tile_coordinate_list& coords //output
    )
    {
        libutil::for_each_ij
        (
            [&](auto& opt_tile, const int row, const int col)
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

                        [&](const data_types::tiles::column_nullifier&)
                        {
                            //Remove all tiles from current column
                            for(int nullified_row = 0; nullified_row < tiles.m; ++nullified_row)
                            {
                                auto& opt_tile = libutil::at(tiles, nullified_row, col);

                                if(!opt_tile)
                                {
                                    continue;
                                }

                                coords.push_back({nullified_row, col});

                                opt_tile = std::nullopt;
                            }
                        },

                        [&](const data_types::tiles::row_nullifier&)
                        {
                            //Remove all tiles from current row
                            for(int nullified_col = 0; nullified_col < tiles.n; ++nullified_col)
                            {
                                auto& opt_tile = libutil::at(tiles, row, nullified_col);

                                if(!opt_tile)
                                {
                                    continue;
                                }

                                coords.push_back({row, nullified_col});

                                opt_tile = std::nullopt;
                            }
                        },

                        [&](const data_types::tiles::number_nullifier&)
                        {
                            //Remove the nullifier tile itself
                            opt_tile = std::nullopt;
                            coords.push_back({row, col});

                            //Get the value of the number tile placed below the
                            //nullifier tile, if any
                            const auto opt_value = [&]() -> std::optional<int>
                            {
                                if(row == 0)
                                {
                                    return std::nullopt;
                                }

                                const auto& opt_below_tile = libutil::at(tiles, row - 1, col);

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
                            libutil::for_each_ij
                            (
                                [&](auto& opt_tile, const int row, const int col)
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
                                    coords.push_back({row, col});
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
}

board::board(data_types::board_tile_array& tiles):
    tiles_(tiles)
{
}

bool board::is_game_over() const
{
    for(int col = 0; col < tiles_.n; ++col)
    {
        if(libutil::at(tiles_, authorized_row_count, col))
        {
            return true;
        }
    }

    return false;
}

int board::get_highest_tile_value() const
{
    auto value = 0;
    for(const auto& opt_tile: tiles_)
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

int board::get_score() const
{
    auto score = 0;
    for(const auto& opt_tile: tiles_)
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

int board::get_free_cell_count() const
{
    auto count = authorized_cell_count;
    for(const auto& opt_tile: tiles_)
    {
        if(opt_tile)
        {
            --count;
        }
    }
    return count;
}

void board::get_targeted_tiles(const board_input& in, data_types::tile_coordinate_list& coords) const
{
    const auto board_tiles = apply_gravity(in.get_tiles(), in.get_layout(), tiles_, nullptr);
    apply_nullifiers(board_tiles, coords);
}

void board::clear()
{
    for(auto& opt_tile: tiles_)
    {
        opt_tile = std::nullopt;
    }
}

void board::drop_input_tiles(const board_input& in, event_list& events)
{
    //Apply gravity on input tiles
    {
        auto drops = data_types::input_tile_drop_list{};
        tiles_ = apply_gravity(in.get_tiles(), in.get_layout(), tiles_, &drops);
        events.push_back(events::input_tile_drop{std::move(drops)});
    }

    auto old_event_count = 0;
    do
    {
        old_event_count = events.size();

        {
            auto nullified_tiles = data_types::tile_coordinate_list{};
            tiles_ = apply_nullifiers(tiles_, nullified_tiles);
            if(!nullified_tiles.empty())
            {
                events.push_back(events::tile_nullification{std::move(nullified_tiles)});
            }
        }

        const auto merges = merge_tiles();
        if(!merges.empty())
        {
            events.push_back(events::tile_merge{merges});
        }

        const auto drops = make_tiles_fall();
        if(!drops.empty())
        {
            events.push_back(events::board_tile_drop{drops});
        }
    } while(old_event_count != events.size());

    events.push_back(events::score_change{get_score()});
}

data_types::board_tile_drop_list board::make_tiles_fall()
{
    data_types::board_tile_drop_list drops;

    for(int col = 0; col < tiles_.n; ++col)
    {
        std::optional<int> opt_empty_cell_row;
        for(int row = 0; row < tiles_.m; ++row) //from bottom to top
        {
            if(const auto& opt_tile = libutil::at(tiles_, row, col))
            {
                if(opt_empty_cell_row) //if the tile is floating
                {
                    libutil::at(tiles_, *opt_empty_cell_row, col) = opt_tile;
                    libutil::at(tiles_, row, col) = std::nullopt;

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

    data_types::board_tile_array tile_layer;

    //select the identical adjacent tiles
    //scan row by row, from the bottom left corner to the top right corner
    libutil::for_each_ij
    (
        [&](const auto& opt_tile, const int row, const int col)
        {
            const auto pnum_tile = std::get_if<data_types::tiles::number>(&*opt_tile);
            if(!pnum_tile)
            {
                return;
            }

            const auto& current_tile = *pnum_tile;

            auto selection = selection_t{}; //fill with unselected
            auto selection_size = 0;

            select_tiles(current_tile.value, row, col, selection, selection_size);

            //if 3 or more tiles are selected
            if(selection_size >= 3)
            {
                //remove the selected tiles from the board
                std::vector<data_types::tile_coordinate> removed_tile_coordinates;
                libutil::for_each_ij
                (
                    [&](auto& opt_tile2, const int row2, const int col2)
                    {
                        if(libutil::at(selection, row2, col2) == selection_state::selected)
                        {
                            assert(opt_tile2.has_value());
                            removed_tile_coordinates.push_back({row2, col2});
                            opt_tile2 = std::nullopt;
                        }
                    },
                    tiles_
                );

                //put the new merged tile on the layer
                auto merged_tile = data_types::tiles::number{current_tile.value + 1};
                libutil::at(tile_layer, row, col) = merged_tile;

                merges.push_back
                (
                    data_types::tile_merge
                    {
                        removed_tile_coordinates,
                        data_types::tile_coordinate{row, col},
                        merged_tile.value
                    }
                );
            }
        },
        tiles_
    );

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
    const int row,
    const int col,
    selection_t& selection,
    int& selection_size
)
{
    libutil::at(selection, row, col) = selection_state::visited;

    const auto& opt_tile = libutil::at(tiles_, row, col);
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

    libutil::at(selection, row, col) = selection_state::selected;
    ++selection_size;

    //above tile
    if
    (
        row + 1 < total_row_count &&
        libutil::at(selection, row + 1, col) == selection_state::unselected
    )
    {
        select_tiles(tile_value, row + 1, col, selection, selection_size);
    }

    //beneath tile
    if
    (
        row >= 1 &&
        libutil::at(selection, row - 1, col) == selection_state::unselected
    )
    {
        select_tiles(tile_value, row - 1, col, selection, selection_size);
    }

    //right tile
    if
    (
        col + 1 < total_column_count &&
        libutil::at(selection, row, col + 1) == selection_state::unselected
    )
    {
        select_tiles(tile_value, row, col + 1, selection, selection_size);
    }

    //left tile
    if
    (
        col >= 1 &&
        libutil::at(selection, row, col - 1) == selection_state::unselected
    )
    {
        select_tiles(tile_value, row, col - 1, selection, selection_size);
    }
}

} //namespace
