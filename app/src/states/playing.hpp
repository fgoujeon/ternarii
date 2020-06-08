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

#ifndef STATES_PLAYING_HPP
#define STATES_PLAYING_HPP

#include "../fsm.hpp"
#include <libgame/game.hpp>
#include <libutil/log.hpp>

namespace states
{

class playing final: public state
{
    public:
        using screen = libview::screens::game;
        using screen_transition = libview::view::screen_transition;

    public:
        playing(fsm& f, screen_transition trans, libgame::data_types::stage stage);

    //Game event handlers
    private:
        void handle_game_event(const libgame::events::start&)
        {
            pscreen_->clear();
        }

        void handle_game_event(const libgame::events::score_change& event)
        {
            pscreen_->set_score(event.score);
        }

        void handle_game_event(const libgame::events::hi_score_change& event)
        {
            pscreen_->set_hi_score(event.score);
        }

        void handle_game_event(const libgame::events::next_input_creation& event)
        {
            pscreen_->create_next_input(event.tiles);
        }

        void handle_game_event(const libgame::events::next_input_insertion&)
        {
            pscreen_->insert_next_input();
            mark_tiles_for_nullification();
            fsm_.database.set_stage_state(stage_, pgame_->get_state());
        }

        void handle_game_event(const libgame::events::input_tile_drop& event)
        {
            pscreen_->drop_input_tiles(event.drops);
        }

        void handle_game_event(const libgame::events::board_tile_drop& event)
        {
            pscreen_->drop_board_tiles(event.drops);
        }

        void handle_game_event(const libgame::events::tile_nullification& event)
        {
            pscreen_->nullify_tiles(event.nullified_tile_coordinates);
        }

        void handle_game_event(const libgame::events::tile_merge& event)
        {
            pscreen_->merge_tiles(event.merges);
        }

        void handle_game_event(const libgame::events::end_of_game&)
        {
            pscreen_->set_game_over_overlay_visible(true);
            fsm_.database.set_stage_state(stage_, pgame_->get_state());
        }

        void handle_game_events(const libgame::event_list& events)
        {
            for(const auto& event: events)
            {
                std::visit
                (
                    [this](const auto& event)
                    {
                        libutil::log::info("[fsm <- game] ", event);
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
            if(!pgame_)
            {
                return;
            }

            game_events_.clear();
            std::invoke(fn, *pgame_, std::forward<Args>(args)..., game_events_);
            handle_game_events(game_events_);
        }

        void mark_tiles_for_nullification()
        {
            targeted_tiles_.clear();
            pgame_->get_targeted_tiles(pscreen_->get_input_layout(), targeted_tiles_);
            pscreen_->mark_tiles_for_nullification(targeted_tiles_);
        }

    private:
        fsm& fsm_;
        const libgame::data_types::stage stage_;
        std::shared_ptr<libview::screens::game> pscreen_;
        std::unique_ptr<libgame::game> pgame_;

        //used by modify_game()
        libgame::event_list game_events_;

        //used by mark_tiles_for_nullification()
        libutil::matrix_coordinate_list targeted_tiles_;
};

} //namespace

#endif
