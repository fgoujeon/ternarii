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

#ifndef STATES_PLAYING_VERSUS_HPP
#define STATES_PLAYING_VERSUS_HPP

#include "../fsm.hpp"
#include <libview/screens/versus_game.hpp>
#include <libgame/pvp_game.hpp>
#include <libutil/log.hpp>

namespace states
{

class playing_versus final: public libutil::fsm::state
{
    public:
        using screen = libview::screens::versus_game;
        using screen_transition = libview::view::screen_transition;

    public:
        playing_versus
        (
            fsm& f,
            screen_transition trans,
            libgame::data_types::stage stage
        );

        ~playing_versus()
        {
        }

        void handle_event(const std::any& /*event*/) override
        {
        }

    //Game event handlers
    private:
        void handle_game_event(const libgame::events::start&)
        {
            pscreen_->clear();
        }

        void handle_game_event(const libgame::events::pvp_score_change& event)
        {
            switch(event.player_index)
            {
                case 0:
                    pscreen_->set_p1_score(event.score);
                    break;
                case 1:
                    pscreen_->set_p2_score(event.score);
                    break;
            }
        }

        void handle_game_event(const libgame::events::pvp_next_input_creation& event)
        {
            switch(event.player_index)
            {
                case 0:
                    pscreen_->create_p1_next_input(event.tiles);
                    break;
                case 1:
                    pscreen_->create_p2_next_input(event.tiles);
                    break;
            }
        }

        void handle_game_event(const libgame::events::pvp_next_input_insertion& event)
        {
            switch(event.player_index)
            {
                case 0:
                    pscreen_->insert_p1_next_input();
                    break;
                case 1:
                    pscreen_->insert_p2_next_input();
                    break;
            }
            mark_tiles_for_nullification(event.player_index);
        }

        void handle_game_event(const libgame::events::pvp_input_tile_drop& event)
        {
            switch(event.player_index)
            {
                case 0:
                    pscreen_->drop_p1_input_tiles(event.drops);
                    break;
                case 1:
                    pscreen_->drop_p2_input_tiles(event.drops);
                    break;
            }
        }

        void handle_game_event(const libgame::events::pvp_board_tile_drop& event)
        {
            switch(event.player_index)
            {
                case 0:
                    pscreen_->drop_p1_board_tiles(event.drops);
                    break;
                case 1:
                    pscreen_->drop_p2_board_tiles(event.drops);
                    break;
            }
        }

        void handle_game_event(const libgame::events::pvp_tile_nullification& event)
        {
            switch(event.player_index)
            {
                case 0:
                    pscreen_->nullify_p1_tiles(event.nullified_tile_coordinates);
                    break;
                case 1:
                    pscreen_->nullify_p2_tiles(event.nullified_tile_coordinates);
                    break;
            }
        }

        void handle_game_event(const libgame::events::pvp_tile_merge& event)
        {
            switch(event.player_index)
            {
                case 0:
                    pscreen_->merge_p1_tiles
                    (
                        event.merges,
                        event.granite_erosions
                    );
                    break;
                case 1:
                    pscreen_->merge_p2_tiles
                    (
                        event.merges,
                        event.granite_erosions
                    );
                    break;
            }
        }

        void handle_game_event(const libgame::events::end_of_game&)
        {
            pscreen_->set_game_over_overlay_visible(true);
        }

        void handle_game_events(const libgame::pvp_game::event_list& events)
        {
            for(const auto& event: events)
            {
                std::visit
                (
                    [this](const auto& event)
                    {
                        //libutil::log::info("[fsm <- game] ", event);
                        handle_game_event(event);
                    },
                    event
                );
            }
        }

    private:
        /*
        Call given libgame::game's modifier function and handle the returned
        events.
        */
        template<class Fn, class... Args>
        void modify_game(Fn&& fn, Args&&... args)
        {
            game_events_.clear();
            std::invoke(fn, game_, std::forward<Args>(args)..., game_events_);
            handle_game_events(game_events_);
        }

        void mark_tiles_for_nullification(const int player_index)
        {
            targeted_tiles_.clear();
            switch(player_index)
            {
                case 0:
                    game_.get_targeted_tiles(player_index, pscreen_->get_p1_input_layout(), targeted_tiles_);
                    pscreen_->mark_p1_tiles_for_nullification(targeted_tiles_);
                    break;
                case 1:
                    game_.get_targeted_tiles(player_index, pscreen_->get_p2_input_layout(), targeted_tiles_);
                    pscreen_->mark_p2_tiles_for_nullification(targeted_tiles_);
                    break;
            };
        }

    private:
        fsm& fsm_;
        libgame::pvp_game game_;
        std::shared_ptr<screen> pscreen_;

        //used by modify_game
        libgame::pvp_game::event_list game_events_;

        //used by mark_tiles_for_nullification
        libutil::matrix_coordinate_list targeted_tiles_;
};

} //namespace

#endif
