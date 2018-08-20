#include "board.hpp"
#include "board_input.hpp"
#include <libgame/element.hpp>
#include <memory>
#include <cmath>
#include <cassert>

namespace libgame
{

board::board():
    highest_unlocked_element_index_(2)
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

std::vector<std::vector<game_change_t>>
board::drop_input(const board_input& in)
{
    std::vector<std::vector<game_change_t>> changes;

    insert_input(in);

    bool changes_happened;
    do
    {
        const auto fall_changes = make_items_fall();
        changes.push_back(fall_changes);

        const auto transmutation_changes = transmute_elements();
        changes.push_back(transmutation_changes);

        changes.push_back({game_changes::score_change{get_score()}});

        changes_happened = !fall_changes.empty() || !transmutation_changes.empty();
    } while(changes_happened);

    return changes;
}

void
board::insert_input(const board_input& in)
{
    //put the input on the upper rows
    for(unsigned int column_index = 0; column_index < board_input::column_count; ++column_index)
    {
        for(unsigned int row_index = 0; row_index < board_input::row_count; ++row_index)
        {
            if(const auto& opt_item = in.item_grid().at(column_index, row_index))
            {
                const auto& item = *opt_item;
                item_grid_.at(column_index, row_count - 2 + row_index) = item;
            }
        }
    }
}

std::vector<game_change_t>
board::make_items_fall()
{
    std::vector<game_change_t> changes;

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
                        board_changes::item_drop
                        {
                            *opt_item,
                            column_index,
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

std::vector<game_change_t>
board::transmute_elements()
{
    std::vector<game_change_t> changes;

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
                        std::vector<std::shared_ptr<element>> removed_elements;
                        for(unsigned int i = 0; i < column_count * row_count; ++i)
                        {
                            if(selection.at(i) == selection_state::SELECTED)
                            {
                                assert(item_grid_.at(i));
                                removed_elements.push_back(*item_grid_.at(i));
                                item_grid_.at(i) = std::nullopt;
                            }
                        }

                        //put the new transmuted item on the layer
                        auto new_element = std::make_shared<element>(element{current_element->value + 1});
                        item_layer.at(column_index, row_index) = new_element;

                        changes.push_back
                        (
                            board_changes::element_transmutation
                            {
                                removed_elements,
                                new_element,
                                column_index,
                                row_index
                            }
                        );

                        if(highest_unlocked_element_index_ < new_element->value)
                        {
                            highest_unlocked_element_index_ = new_element->value;
                            changes.push_back
                            (
                                game_changes::element_unlocking
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