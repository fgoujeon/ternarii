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

#include "private_board.hpp"
#include <libgame/data_types.hpp>
#include <libutil/overload.hpp>
#include <memory>
#include <cmath>
#include <cassert>

namespace libgame
{

namespace
{
}

private_board::private_board(board& brd):
    board_(brd)
{
}

libutil::matrix_coordinate_list private_board::get_targeted_tiles
(
    const data_types::input_tile_matrix& input_tiles,
    const data_types::input_layout& input_layout
) const
{
    const auto result = apply_gravity_on_input
    (
        board_,
        input_tiles,
        input_layout
    );
    auto result2 = apply_nullifiers(result.brd);
    return result2.nullified_tiles_coords;
}

void private_board::drop_input_tiles
(
    const data_types::input_tile_matrix& input_tiles,
    const data_types::input_layout& input_layout,
    event_list& events
)
{
    //Apply gravity on input tiles
    {
        auto result = apply_gravity_on_input
        (
            board_,
            input_tiles,
            input_layout
        );
        board_ = result.brd;
        events.push_back(events::input_tile_drop{std::move(result.drops)});
    }

    auto old_event_count = 0;
    do
    {
        old_event_count = events.size();

        //Apply nullifier tiles
        {
            auto result = apply_nullifiers(board_);
            board_ = std::move(result.brd);
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
            auto result = apply_adders(board_);
            board_ = std::move(result.brd);
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
        const auto merges = merge_tiles();

        //Decrease thickness of granite tiles
        if(!merges.empty())
        {
            const auto result = apply_merges_on_granites(board_, merges);

            board_ = result.brd;

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
            auto result = apply_gravity(board_);
            board_ = std::move(result.brd);
            if(!result.drops.empty())
                events.push_back(events::board_tile_drop{std::move(result.drops)});
        }
    } while(old_event_count != events.size());

    events.push_back(events::score_change{get_score(board_)});
}

data_types::tile_merge_list private_board::merge_tiles()
{
    data_types::tile_merge_list merges;

    data_types::board_tile_matrix tile_layer;

    //Select the identical adjacent tiles.
    //Scan row by row, from the bottom left corner to the top right corner.
    for(int row = 0; row < board_.tiles.rows; ++row)
    {
        for(int col = 0; col < board_.tiles.cols; ++col)
        {
            const auto& opt_tile = at(board_.tiles, col, row);
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
                    board_.tiles
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
            board_.tiles,
            tile_layer
        );
    }

    return merges;
}

void private_board::select_tiles
(
    const int tile_value,
    const int col,
    const int row,
    selection_t& selection,
    int& selection_size
)
{
    at(selection, col, row) = selection_state::visited;

    const auto& opt_tile = at(board_.tiles, col, row);
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
