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
#include <libgame/game.hpp>
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

        void handle_event(const std::any& event) override
        {
            if(const auto pevent = std::any_cast<events::iteration>(&event))
            {
                p1_game_.advance(pevent->elapsed_s);
                p2_game_.advance(pevent->elapsed_s);
            }
        }

    //P1 game event handlers
    private:
        void handle_p1_game_event(const libgame::events::start&)
        {
            pscreen_->clear();
        }

        void handle_p1_game_event(const libgame::events::move_count_change&)
        {
        }

        void handle_p1_game_event(const libgame::events::score_change& event)
        {
            pscreen_->set_p1_score(event.score);
        }

        void handle_p1_game_event(const libgame::events::hi_score_change&)
        {
        }

        void handle_p1_game_event(const libgame::events::next_input_creation& event)
        {
            pscreen_->create_p1_next_input(event.tiles);
        }

        void handle_p1_game_event(const libgame::events::next_input_insertion&)
        {
            pscreen_->insert_p1_next_input();
            mark_p1_tiles_for_nullification();
        }

        void handle_p1_game_event(const libgame::events::input_tile_drop& event)
        {
            pscreen_->drop_p1_input_tiles(event.drops);
        }

        void handle_p1_game_event(const libgame::events::board_tile_drop& event)
        {
            pscreen_->drop_p1_board_tiles(event.drops);
        }

        void handle_p1_game_event(const libgame::events::tile_nullification& event)
        {
            pscreen_->nullify_p1_tiles(event.nullified_tile_coordinates);
        }

        void handle_p1_game_event(const libgame::events::tile_merge& event)
        {
            pscreen_->merge_p1_tiles
            (
                event.merges,
                event.granite_erosions
            );
        }

        void handle_p1_game_event(const libgame::events::end_of_game&)
        {
            pscreen_->set_game_over_overlay_visible(true);
        }

        void handle_p1_game_events(const libgame::event_list& events)
        {
            for(const auto& event: events)
            {
                std::visit
                (
                    [this](const auto& event)
                    {
                        libutil::log::info("[fsm <- game] ", event);
                        handle_p1_game_event(event);
                    },
                    event
                );
            }
        }

    //P2 game event handlers
    private:
        void handle_p2_game_event(const libgame::events::start&)
        {
            //pscreen_->clear();
        }

        void handle_p2_game_event(const libgame::events::move_count_change&)
        {
        }

        void handle_p2_game_event(const libgame::events::score_change& event)
        {
            pscreen_->set_p2_score(event.score);
        }

        void handle_p2_game_event(const libgame::events::hi_score_change&)
        {
        }

        void handle_p2_game_event(const libgame::events::next_input_creation& event)
        {
            pscreen_->create_p2_next_input(event.tiles);
        }

        void handle_p2_game_event(const libgame::events::next_input_insertion&)
        {
            pscreen_->insert_p2_next_input();
            mark_p2_tiles_for_nullification();
        }

        void handle_p2_game_event(const libgame::events::input_tile_drop& event)
        {
            pscreen_->drop_p2_input_tiles(event.drops);
        }

        void handle_p2_game_event(const libgame::events::board_tile_drop& event)
        {
            pscreen_->drop_p2_board_tiles(event.drops);
        }

        void handle_p2_game_event(const libgame::events::tile_nullification& event)
        {
            pscreen_->nullify_p2_tiles(event.nullified_tile_coordinates);
        }

        void handle_p2_game_event(const libgame::events::tile_merge& event)
        {
            pscreen_->merge_p2_tiles
            (
                event.merges,
                event.granite_erosions
            );
        }

        void handle_p2_game_event(const libgame::events::end_of_game&)
        {
            pscreen_->set_game_over_overlay_visible(true);
        }

        void handle_p2_game_events(const libgame::event_list& events)
        {
            for(const auto& event: events)
            {
                std::visit
                (
                    [this](const auto& event)
                    {
                        libutil::log::info("[fsm <- game] ", event);
                        handle_p2_game_event(event);
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
        void modify_p1_game(Fn&& fn, Args&&... args)
        {
            static auto game_events = libgame::event_list{};
            game_events.clear();
            std::invoke(fn, p1_game_, std::forward<Args>(args)..., game_events);
            handle_p1_game_events(game_events);
        }

        /*
        Call given libgame::game's modifier function and handle the returned
        events.
        */
        template<class Fn, class... Args>
        void modify_p2_game(Fn&& fn, Args&&... args)
        {
            static auto game_events = libgame::event_list{};
            game_events.clear();
            std::invoke(fn, p2_game_, std::forward<Args>(args)..., game_events);
            handle_p2_game_events(game_events);
        }

        void mark_p1_tiles_for_nullification()
        {
            static auto targeted_tiles = libutil::matrix_coordinate_list{};
            targeted_tiles.clear();
            p1_game_.get_targeted_tiles(pscreen_->get_p1_input_layout(), targeted_tiles);
            pscreen_->mark_p1_tiles_for_nullification(targeted_tiles);
        }

        void mark_p2_tiles_for_nullification()
        {
            static auto targeted_tiles = libutil::matrix_coordinate_list{};
            targeted_tiles.clear();
            p2_game_.get_targeted_tiles(pscreen_->get_p2_input_layout(), targeted_tiles);
            pscreen_->mark_p2_tiles_for_nullification(targeted_tiles);
        }

    private:
        fsm& fsm_;
        libgame::game p1_game_;
        libgame::game p2_game_;
        std::shared_ptr<screen> pscreen_;
};

} //namespace

#endif
