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
    for(int col = 0; col < tile_array_.m; ++col)
    {
        if(libutil::at(tile_array_, col, authorized_row_count))
        {
            return true;
        }
    }

    return false;
}

int board::get_highest_tile_value() const
{
    auto value = 0;
    for(const auto& opt_tile: tile_array_)
    {
        if(opt_tile)
        {
            if(const auto pnum_tile = std::get_if<data_types::number_tile>(&*opt_tile))
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
    for(const auto& opt_tile: tile_array_)
    {
        if(opt_tile)
        {
            if(const auto pnum_tile = std::get_if<data_types::number_tile>(&*opt_tile))
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
    for(const auto& opt_tile: tile_array_)
    {
        if(opt_tile)
        {
            --count;
        }
    }
    return count;
}

void board::clear()
{
    for(auto& opt_tile: tile_array_)
    {
        opt_tile = std::nullopt;
    }
}

void board::drop_input_tiles(const board_input& in, event_list& events)
{
    events.push_back
    (
        events::input_tile_drop
        {
            drop_input_tiles_only(in)
        }
    );

    bool events_happened;
    do
    {
        const auto drops = make_tiles_fall();
        if(!drops.empty())
        {
            events.push_back(events::board_tile_drop{drops});
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

data_types::input_tile_drop_list board::drop_input_tiles_only(const board_input& in)
{
    auto drops = data_types::input_tile_drop_list{};

    const auto& tiles = in.get_tiles();
    const auto& layout = in.get_layout();

    //Make tiles fall from lowest to highest row of laid out input.
    for(auto input_row_index = 0; input_row_index < 2; ++input_row_index)
    {
        libutil::for_each_ij
        (
            [&](auto& opt_tile, const int col, const int row)
            {
                if(!opt_tile)
                {
                    return;
                }

                const auto coord = get_tile_coordinate(layout, {col, row});

                if(coord.row_index != input_row_index)
                {
                    return;
                }

                const auto opt_dst_row_index = libcommon::data_types::get_lowest_empty_cell
                (
                    tile_array_,
                    coord.column_index
                );

                if(!opt_dst_row_index)
                {
                    return;
                }

                libutil::at(tile_array_, coord.column_index, *opt_dst_row_index) = opt_tile;

                drops.push_back
                (
                    data_types::input_tile_drop
                    {
                        {col, row},
                        {coord.column_index, *opt_dst_row_index}
                    }
                );
            },
            tiles
        );
    }

    return drops;
}

data_types::board_tile_drop_list board::make_tiles_fall()
{
    data_types::board_tile_drop_list drops;

    for(int column_index = 0; column_index < tile_array_.m; ++column_index)
    {
        std::optional<int> opt_empty_cell_row_index;
        for(int row_index = 0; row_index < tile_array_.n; ++row_index) //from bottom to top
        {
            if(const auto opt_tile = libutil::at(tile_array_, column_index, row_index))
            {
                if(opt_empty_cell_row_index) //if the tile is floating
                {
                    libutil::at(tile_array_, column_index, row_index) = std::nullopt;
                    libutil::at(tile_array_, column_index, *opt_empty_cell_row_index) = opt_tile;

                    drops.push_back
                    (
                        data_types::board_tile_drop
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
            const auto& opt_tile = libutil::at(tile_array_, column_index, row_index);

            const auto pnum_tile = std::get_if<data_types::number_tile>(&*opt_tile);
            if(!pnum_tile)
            {
                break;
            }

            const auto& current_tile = *pnum_tile;

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
                        if(libutil::at(selection, column_index2, row_index2) == selection_state::selected)
                        {
                            assert(libutil::at(tile_array_, column_index2, row_index2));
                            removed_tile_coordinates.push_back
                            (
                                data_types::tile_coordinate
                                {
                                    column_index2,
                                    row_index2
                                }
                            );
                            libutil::at(tile_array_, column_index2, row_index2) = std::nullopt;
                        }
                    }
                }

                //put the new merged tile on the layer
                auto merged_tile = data_types::number_tile{current_tile.value + 1};
                libutil::at(tile_layer, column_index, row_index) = merged_tile;

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
            tile_array_,
            tile_layer
        );
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
    libutil::at(selection, column_index, row_index) = selection_state::visited;

    const auto& opt_tile = libutil::at(tile_array_, column_index, row_index);
    if(!opt_tile)
    {
        return;
    }

    const auto pnum_tile = std::get_if<data_types::number_tile>(&*opt_tile);
    if(!pnum_tile)
    {
        return;
    }

    const auto& tile = *pnum_tile;

    if(tile.value != tile_value)
    {
        return;
    }

    libutil::at(selection, column_index, row_index) = selection_state::selected;
    ++selection_size;

    //above tile
    if
    (
        row_index + 1 < total_row_count &&
        libutil::at(selection, column_index, row_index + 1) == selection_state::unselected
    )
    {
        select_tiles(tile_value, column_index, row_index + 1, selection, selection_size);
    }

    //beneath tile
    if
    (
        row_index >= 1 &&
        libutil::at(selection, column_index, row_index - 1) == selection_state::unselected
    )
    {
        select_tiles(tile_value, column_index, row_index - 1, selection, selection_size);
    }

    //right tile
    if
    (
        column_index + 1 < total_column_count &&
        libutil::at(selection, column_index + 1, row_index) == selection_state::unselected
    )
    {
        select_tiles(tile_value, column_index + 1, row_index, selection, selection_size);
    }

    //left tile
    if
    (
        column_index >= 1 &&
        libutil::at(selection, column_index - 1, row_index) == selection_state::unselected
    )
    {
        select_tiles(tile_value, column_index - 1, row_index, selection, selection_size);
    }
}

} //namespace
