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
#include <memory>
#include <cmath>
#include <cassert>

namespace libgame
{

board::board
(
    data_types::board_tile_array& tiles,
    int& hi_score
):
    tile_array_(tiles),
    hi_score_(hi_score)
{
}

bool board::is_game_over() const
{
    for(int i = 0; i < total_column_count; ++i)
    {
        if(tile_array_[i][authorized_row_count])
        {
            return true;
        }
    }

    return false;
}

int board::get_highest_tile_value() const
{
    auto value = 0;
    for(const auto& cell_column: tile_array_)
    {
        for(const auto& opt_tile: cell_column)
        {
            if(opt_tile)
            {
                value = std::max(value, opt_tile->value);
            }
        }
    }
    return value;
}

int board::get_score() const
{
    auto score = 0;
    for(const auto& cell_column: tile_array_)
    {
        for(const auto& opt_tile: cell_column)
        {
            if(opt_tile)
            {
                score += std::pow(3, opt_tile->value);
            }
        }
    }
    return score;
}

int board::get_free_cell_count() const
{
    auto count = authorized_cell_count;

    for(int row_index = 0; row_index < authorized_row_count; ++row_index)
    {
        for(int column_index = 0; column_index < authorized_column_count; ++column_index)
        {
            if(tile_array_[column_index][row_index])
            {
                --count;
            }
        }
    }

    return count;
}

void board::clear()
{
    for(auto& cell_column: tile_array_)
    {
        for(auto& opt_tile: cell_column)
        {
            opt_tile = std::nullopt;
        }
    }
}

void board::drop_input(const board_input& in, event_list& events)
{
    events.push_back(insert_input(in));

    bool events_happened;
    do
    {
        const auto drops = make_tiles_fall();
        if(!drops.empty())
        {
            events.push_back(events::tile_drop{drops});
        }

        const auto merges = merge_tiles();
        if(!merges.empty())
        {
            events.push_back(events::tile_merge{merges});
        }

        events_happened = !drops.empty() || !merges.empty();
    } while(events_happened);

    events.push_back(events::score_change{get_score()});

    if(hi_score_ < get_score())
    {
        hi_score_ = get_score();
        events.push_back(events::hi_score_change{hi_score_});
    }
}

events::input_insertion board::insert_input(const board_input& in)
{
    //put the input on the upper rows

    const auto& state = in.get_state();

    const int x0 = state.x_offset + (state.rotation == 2 ? 1 : 0);
    const int y0 = total_row_count - 2 + (state.rotation == 1 ? 1 : 0);

    const int x1 = state.x_offset + (state.rotation == 0 ? 1 : 0);
    const int y1 = total_row_count - 2 + (state.rotation == 3 ? 1 : 0);

    tile_array_[x0][y0] = state.tiles[0];
    tile_array_[x1][y1] = state.tiles[1];

    return events::input_insertion{x0, y0, x1, y1};
}

data_types::tile_drop_list board::make_tiles_fall()
{
    data_types::tile_drop_list drops;

    for(int column_index = 0; column_index < total_column_count; ++column_index)
    {
        std::optional<int> opt_empty_cell_row_index;
        for(int row_index = 0; row_index < total_row_count; ++row_index) //from bottom to top
        {
            if(const auto opt_tile = tile_array_[column_index][row_index])
            {
                if(opt_empty_cell_row_index) //if the tile is floating
                {
                    tile_array_[column_index][row_index] = std::nullopt;
                    tile_array_[column_index][*opt_empty_cell_row_index] = opt_tile;

                    drops.push_back
                    (
                        data_types::tile_drop
                        {
                            column_index,
                            row_index,
                            *opt_empty_cell_row_index
                        }
                    );

                    ++*opt_empty_cell_row_index;
                }
            }
            else
            {
                if(!opt_empty_cell_row_index)
                {
                    opt_empty_cell_row_index = row_index;
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
    for(int row_index = 0; row_index < total_row_count; ++row_index)
    {
        for(int column_index = 0; column_index < total_column_count; ++column_index)
        {
            const auto& opt_tile = tile_array_[column_index][row_index];

            if(opt_tile)
            {
                const auto& current_tile = *opt_tile;

                selection_t selection = {}; //fill with unselected
                int selection_size = 0;

                select_tiles(current_tile.value, column_index, row_index, selection, selection_size);

                //if 3 or more tiles are selected
                if(selection_size >= 3)
                {
                    //remove the selected tiles from the board
                    std::vector<data_types::tile_coordinate> removed_tile_coordinates;
                    for(int row_index2 = 0; row_index2 < total_row_count; ++row_index2)
                    {
                        for(int column_index2 = 0; column_index2 < total_column_count; ++column_index2)
                        {
                            if(selection[column_index2][row_index2] == selection_state::selected)
                            {
                                assert(tile_array_[column_index2][row_index2]);
                                removed_tile_coordinates.push_back
                                (
                                    data_types::tile_coordinate
                                    {
                                        column_index2,
                                        row_index2
                                    }
                                );
                                tile_array_[column_index2][row_index2] = std::nullopt;
                            }
                        }
                    }

                    //put the new merged tile on the layer
                    auto merged_tile = data_types::tile{current_tile.value + 1};
                    tile_layer[column_index][row_index] = merged_tile;

                    merges.push_back
                    (
                        data_types::tile_merge
                        {
                            removed_tile_coordinates,
                            data_types::tile_coordinate{column_index, row_index},
                            merged_tile.value
                        }
                    );
                }
            }
        }
    }

    if(!merges.empty())
    {
        //overlay the tile layer to the tile array of the board
        for(auto x = 0; x < total_column_count; ++x)
        {
            for(auto y = 0; y < total_row_count; ++y)
            {
                if(tile_layer[x][y])
                {
                    assert(!tile_array_[x][y]);
                    tile_array_[x][y] = tile_layer[x][y];
                }
            }
        }
    }

    return merges;
}

void board::select_tiles
(
    const int tile_value,
    const int column_index,
    const int row_index,
    selection_t& selection,
    int& selection_size
)
{
    selection[column_index][row_index] = selection_state::visited;

    if(auto opt_tile = tile_array_[column_index][row_index])
    {
        auto tile = *opt_tile;
        if(tile.value == tile_value)
        {
            selection[column_index][row_index] = selection_state::selected;
            ++selection_size;

            //above tile
            if
            (
                row_index + 1 < total_row_count &&
                selection[column_index][row_index + 1] == selection_state::unselected
            )
            {
                select_tiles(tile_value, column_index, row_index + 1, selection, selection_size);
            }

            //beneath tile
            if
            (
                row_index >= 1 &&
                selection[column_index][row_index - 1] == selection_state::unselected
            )
            {
                select_tiles(tile_value, column_index, row_index - 1, selection, selection_size);
            }

            //right tile
            if
            (
                column_index + 1 < total_column_count &&
                selection[column_index + 1][row_index] == selection_state::unselected
            )
            {
                select_tiles(tile_value, column_index + 1, row_index, selection, selection_size);
            }

            //left tile
            if
            (
                column_index >= 1 &&
                selection[column_index - 1][row_index] == selection_state::unselected
            )
            {
                select_tiles(tile_value, column_index - 1, row_index, selection, selection_size);
            }
        }
    }
}

} //namespace
