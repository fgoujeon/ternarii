#include "board.hpp"
#include "board_input.hpp"
#include <libgame/data_types.hpp>
#include <memory>
#include <cmath>
#include <cassert>

namespace libgame
{

board::board():
    highest_unlocked_element_index_(3)
{
}

bool
board::is_game_over() const
{
    for(unsigned int i = 0; i < column_count; ++i)
        if(tile_grid_[i][7]) return true;

    return false;
}

unsigned int
board::get_score() const
{
    auto score = 0u;
    for(const auto& cell_column: tile_grid_)
        for(const auto& opt_tile: cell_column)
            if(opt_tile)
                score += std::pow(3, opt_tile->value);
    return score;
}

std::vector<std::vector<event>>
board::drop_input(const board_input& in)
{
    std::vector<std::vector<event>> changes;

    changes.push_back({insert_input(in)});

    bool changes_happened;
    do
    {
        const auto fall_changes = make_tiles_fall();
        changes.push_back(fall_changes);

        const auto transmutation_changes = transmute_elements();
        changes.push_back(transmutation_changes);

        changes.push_back({events::score_change{get_score()}});

        changes_happened = !fall_changes.empty() || !transmutation_changes.empty();
    } while(changes_happened);

    return changes;
}

events::input_insertion
board::insert_input(const board_input& in)
{
    //put the input on the upper rows

    const auto tiles = in.get_tiles();
    const auto x_offset = in.get_x_offset();
    const auto rotation = in.get_rotation();

    const unsigned int x0 = x_offset + (rotation == 2 ? 1 : 0);
    const unsigned int y0 = row_count - 2 + (rotation == 1 ? 1 : 0);

    const unsigned int x1 = x_offset + (rotation == 0 ? 1 : 0);
    const unsigned int y1 = row_count - 2 + (rotation == 3 ? 1 : 0);

    tile_grid_[x0][y0] = tiles[0];
    tile_grid_[x1][y1] = tiles[1];

    return events::input_insertion{x0, y0, x1, y1};
}

std::vector<event>
board::make_tiles_fall()
{
    std::vector<event> changes;

    for(unsigned int column_index = 0; column_index < column_count; ++column_index)
    {
        std::optional<unsigned int> opt_empty_cell_row_index;
        for(unsigned int row_index = 0; row_index < row_count; ++row_index) //from bottom to top
        {
            if(const auto opt_tile = tile_grid_[column_index][row_index])
            {
                if(opt_empty_cell_row_index) //if the tile is floating
                {
                    tile_grid_[column_index][row_index] = std::nullopt;
                    tile_grid_[column_index][*opt_empty_cell_row_index] = opt_tile;

                    changes.push_back
                    (
                        events::tile_drop
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
                    opt_empty_cell_row_index = row_index;
            }
        }
    }

    return changes;
}

std::vector<event>
board::transmute_elements()
{
    std::vector<event> changes;

    grid_t tile_layer;

    //select the identical adjacent tiles
    //scan row by row, from the bottom left corner to the top right corner
    for(unsigned int row_index = 0; row_index < row_count; ++row_index)
    {
        for(unsigned int column_index = 0; column_index < column_count; ++column_index)
        {
            const auto& opt_element = tile_grid_[column_index][row_index];

            if(opt_element)
            {
                const auto& current_element = *opt_element;

                selection_t selection = {}; //fill with unselected
                unsigned int selection_size = 0;

                select_elements(current_element.value, column_index, row_index, selection, selection_size);

                //if 3 or more tiles are selected
                if(selection_size >= 3)
                {
                    //remove the selected tiles from the board
                    std::vector<tile_coordinate> removed_tile_coordinates;
                    for(unsigned int row_index2 = 0; row_index2 < row_count; ++row_index2)
                    {
                        for(unsigned int column_index2 = 0; column_index2 < column_count; ++column_index2)
                        {
                            if(selection[column_index2][row_index2] == selection_state::SELECTED)
                            {
                                assert(tile_grid_[column_index2][row_index2]);
                                removed_tile_coordinates.push_back
                                (
                                    tile_coordinate
                                    {
                                        column_index2,
                                        row_index2
                                    }
                                );
                                tile_grid_[column_index2][row_index2] = std::nullopt;
                            }
                        }
                    }

                    //put the new transmuted tile on the layer
                    auto new_element = tile{current_element.value + 1};
                    tile_layer[column_index][row_index] = new_element;

                    changes.push_back
                    (
                        events::element_transmutation
                        {
                            removed_tile_coordinates,
                            tile_coordinate{column_index, row_index},
                            new_element.value
                        }
                    );

                    if(highest_unlocked_element_index_ < new_element.value)
                    {
                        highest_unlocked_element_index_ = new_element.value;
                        changes.push_back
                        (
                            events::element_unlocking
                            {
                                highest_unlocked_element_index_
                            }
                        );
                    }
                }
            }
        }
    }

    if(!changes.empty())
    {
        //overlay the tile layer to the tile array of the board
        for(auto x = 0; x < column_count; ++x)
        {
            for(auto y = 0; y < row_count; ++y)
            {
                if(tile_layer[x][y])
                {
                    assert(!tile_grid_[x][y]);
                    tile_grid_[x][y] = tile_layer[x][y];
                }
            }
        }
    }

    return changes;
}

void
board::select_elements
(
    const unsigned int element_value,
    const unsigned int column_index,
    const unsigned int row_index,
    selection_t& selection,
    unsigned int& selection_size
)
{
    selection[column_index][row_index] = selection_state::VISITED;

    if(auto opt_tile = tile_grid_[column_index][row_index])
    {
        auto tile = *opt_tile;
        if(tile.value == element_value)
        {
            selection[column_index][row_index] = selection_state::SELECTED;
            ++selection_size;

            //above tile
            if
            (
                row_index + 1 < row_count &&
                selection[column_index][row_index + 1] == selection_state::UNSELECTED
            )
            {
                select_elements(element_value, column_index, row_index + 1, selection, selection_size);
            }

            //beneath tile
            if
            (
                row_index >= 1 &&
                selection[column_index][row_index - 1] == selection_state::UNSELECTED
            )
            {
                select_elements(element_value, column_index, row_index - 1, selection, selection_size);
            }

            //right tile
            if
            (
                column_index + 1 < column_count &&
                selection[column_index + 1][row_index] == selection_state::UNSELECTED
            )
            {
                select_elements(element_value, column_index + 1, row_index, selection, selection_size);
            }

            //left tile
            if
            (
                column_index >= 1 &&
                selection[column_index - 1][row_index] == selection_state::UNSELECTED
            )
            {
                select_elements(element_value, column_index - 1, row_index, selection, selection_size);
            }
        }
    }
}

} //namespace libgame
