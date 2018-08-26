#include "board.hpp"
#include "board_input.hpp"
#include <libgame/element.hpp>
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
    {
        if(item_grid_.at(i, 7)) return true;
    }

    return false;
}

unsigned int
board::get_score() const
{
    auto score = 0u;
    for(const auto& opt_item: item_grid_)
    {
        if(opt_item)
            score += std::pow(3, (*opt_item)->value);
    }
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
        const auto fall_changes = make_items_fall();
        changes.push_back(fall_changes);

        const auto transmutation_changes = transmute_elements();
        changes.push_back(transmutation_changes);

        changes.push_back({events::score_change{get_score()}});

        changes_happened = !fall_changes.empty() || !transmutation_changes.empty();
    } while(changes_happened);

    return changes;
}

events::input_introduction
board::insert_input(const board_input& in)
{
    //put the input on the upper rows

    const auto items = in.get_items();
    const auto x_offset = in.get_x_offset();
    const auto rotation = in.get_rotation();

    const unsigned int x0 = x_offset + (rotation == 2 ? 1 : 0);
    const unsigned int y0 = row_count - 2 + (rotation == 1 ? 1 : 0);

    const unsigned int x1 = x_offset + (rotation == 0 ? 1 : 0);
    const unsigned int y1 = row_count - 2 + (rotation == 3 ? 1 : 0);

    item_grid_.at(x0, y0) = items[0];
    item_grid_.at(x1, y1) = items[1];

    return events::input_introduction{x0, y0, x1, y1};
}

std::vector<event>
board::make_items_fall()
{
    std::vector<event> changes;

    for(unsigned int column_index = 0; column_index < column_count; ++column_index)
    {
        std::optional<unsigned int> opt_empty_cell_row_index;
        for(unsigned int row_index = 0; row_index < row_count; ++row_index) //from bottom to top
        {
            if(const auto opt_item = item_grid_.at(column_index, row_index))
            {
                if(opt_empty_cell_row_index) //if the item is floating
                {
                    item_grid_.at(column_index, row_index) = std::nullopt;
                    item_grid_.at(column_index, *opt_empty_cell_row_index) = opt_item;

                    changes.push_back
                    (
                        events::item_drop
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

    grid_t item_layer;

    //scan row by row, from the bottom left corner to the top right corner
    for(unsigned int row_index = 0; row_index < row_count; ++row_index)
    {
        for(unsigned int column_index = 0; column_index < column_count; ++column_index)
        {
            const std::optional<std::shared_ptr<element>>& opt_element = item_grid_.at(column_index, row_index);

            if(opt_element)
            {
                const std::shared_ptr<element> current_element = *opt_element;

                //select the identical adjacent items
                if(current_element->value < 11)
                {
                    selection_t selection;
                    unsigned int selection_size = 0;

                    //clear the selection
                    selection.fill(selection_state::UNSELECTED);

                    select_elements(current_element->value, column_index, row_index, selection, selection_size);

                    //if 3 or more items are selected
                    if(selection_size >= 3)
                    {
                        //remove the selected items from the board
                        std::vector<tile_coordinate> removed_tile_coordinates;
                        for(unsigned int row_index2 = 0; row_index2 < row_count; ++row_index2)
                        {
                            for(unsigned int column_index2 = 0; column_index2 < column_count; ++column_index2)
                            {
                                if(selection.at(column_index2, row_index2) == selection_state::SELECTED)
                                {
                                    assert(item_grid_.at(column_index2, row_index2));
                                    removed_tile_coordinates.push_back
                                    (
                                        tile_coordinate
                                        {
                                            column_index2,
                                            row_index2
                                        }
                                    );
                                    item_grid_.at(column_index2, row_index2) = std::nullopt;
                                }
                            }
                        }

                        //put the new transmuted item on the layer
                        auto new_element = std::make_shared<element>(element{current_element->value + 1});
                        item_layer.at(column_index, row_index) = new_element;

                        changes.push_back
                        (
                            events::element_transmutation
                            {
                                removed_tile_coordinates,
                                tile_coordinate{column_index, row_index},
                                new_element->value
                            }
                        );

                        if(highest_unlocked_element_index_ < new_element->value)
                        {
                            highest_unlocked_element_index_ = new_element->value;
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
    }

    if(!changes.empty())
    {
        //overlay the item layer to the item array of the board
        for(unsigned int i = 0; i < column_count * row_count; ++i)
        {
            if(item_layer.at(i))
            {
                assert(!item_grid_.at(i));
                item_grid_.at(i) = item_layer.at(i);
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
    selection.at(column_index, row_index) = selection_state::VISITED;

    if(auto opt_item = item_grid_.at(column_index, row_index))
    {
        auto item = *opt_item;
        if(item->value == element_value)
        {
            selection.at(column_index, row_index) = selection_state::SELECTED;
            ++selection_size;

            //above item
            if
            (
                row_index + 1 < row_count &&
                selection.at(column_index, row_index + 1) == selection_state::UNSELECTED
            )
            {
                select_elements(element_value, column_index, row_index + 1, selection, selection_size);
            }

            //beneath item
            if
            (
                row_index >= 1 &&
                selection.at(column_index, row_index - 1) == selection_state::UNSELECTED
            )
            {
                select_elements(element_value, column_index, row_index - 1, selection, selection_size);
            }

            //right item
            if
            (
                column_index + 1 < column_count &&
                selection.at(column_index + 1, row_index) == selection_state::UNSELECTED
            )
            {
                select_elements(element_value, column_index + 1, row_index, selection, selection_size);
            }

            //left item
            if
            (
                column_index >= 1 &&
                selection.at(column_index - 1, row_index) == selection_state::UNSELECTED
            )
            {
                select_elements(element_value, column_index - 1, row_index, selection, selection_size);
            }
        }
    }
}

} //namespace libgame
