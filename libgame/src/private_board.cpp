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
        const auto merges = [&]
        {
            auto result = apply_merges(board_);
            board_ = std::move(result.brd);
            return result.merges;
        }();

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

} //namespace
