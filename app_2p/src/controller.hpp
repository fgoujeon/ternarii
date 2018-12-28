/*
Copyright 2018 Florian Goujeon

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

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "conversion.hpp"
#include <libgame_2p/game.hpp>
#include <libview_2p/view.hpp>

class controller
{
    public:
        controller():
            view_([this](const libview_2p::event& event){handle_view_event(event);})
        {
            handle_game_events(game_.start());
        }

        bool must_quit() const
        {
            return view_.must_quit();
        }

        void iterate()
        {
            view_.iterate();
        }

        void set_window_size(const unsigned int width, const unsigned int height)
        {
            view_.set_window_size(width, height);
        }

    private:
        void handle_game_event(const libgame_2p::events::start& event)
        {
            view_.clear();
        }

        void handle_game_event(const libgame_2p::events::score_change& event)
        {
            view_.set_score(event.player_index, event.score);
        }

        void handle_game_event(const libgame_2p::events::next_input_creation& event)
        {
            view_.create_next_input(event.player_index, event.tiles[0].value, event.tiles[1].value);
        }

        void handle_game_event(const libgame_2p::events::next_input_insertion& event)
        {
            view_.insert_next_input(event.player_index, event.x_offset, event.rotation);
        }

        void handle_game_event(const libgame_2p::events::input_layout_change& event)
        {
            view_.set_input_x_offset(event.player_index, event.x_offset);
            view_.set_input_rotation(event.player_index, event.rotation);
        }

        void handle_game_event(const libgame_2p::events::input_insertion& event)
        {
            view_.insert_input
            (
                event.player_index,
                event.tile0_dst_column_index,
                event.tile0_dst_row_index,
                event.tile1_dst_column_index,
                event.tile1_dst_row_index
            );
        }

        void handle_game_event(const libgame_2p::events::tile_drop& event)
        {
            view_.drop_tiles(event.player_index, conversion::to_view(event.drops));
        }

        void handle_game_event(const libgame_2p::events::tile_merge& event)
        {
            view_.merge_tiles(event.player_index, conversion::to_view(event.merges));
        }

        void handle_game_event(const libgame_2p::events::end_of_game&)
        {
            view_.set_game_over_screen_visible(true);
        }

        void handle_game_events(const libgame_2p::event_list& events)
        {
            for(const auto& event: events)
            {
                std::visit
                (
                    [this](const auto& event)
                    {
                        //std::cout << event << '\n';
                        handle_game_event(event);
                    },
                    event
                );
            }
        }

    private:
        void handle_view_event2(const libview_2p::events::left_shift_request&)
        {
            handle_game_events(game_.shift_input_left());
        }

        void handle_view_event2(const libview_2p::events::right_shift_request&)
        {
            handle_game_events(game_.shift_input_right());
        }

        void handle_view_event2(const libview_2p::events::clockwise_rotation_request&)
        {
            handle_game_events(game_.rotate_input());
        }

        void handle_view_event2(const libview_2p::events::drop_request&)
        {
            handle_game_events(game_.drop_input());
        }

        void handle_view_event2(const libview_2p::events::clear_request&)
        {
            handle_game_events(game_.start());
        }

        void handle_view_event(const libview_2p::event& event)
        {
            std::visit
            (
                [this](const auto& event)
                {
                    handle_view_event2(event);
                },
                event
            );
        }

    private:
        libgame_2p::game game_;
        libview_2p::view view_;
};

#endif
