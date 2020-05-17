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

#ifndef NDEBUG
#include <iostream>
#endif

namespace states
{

class playing final: public state
{
    public:
        playing(fsm& ctx):
            fsm_(ctx)
        {
            //load game state from database
            const auto& opt_game_state = fsm_.database.get_game_state();

            //create game
            if(opt_game_state)
            {
                pgame_ = std::make_unique<libgame::game>(*opt_game_state);

                //initialize view
                fsm_.game_screen.set_score(pgame_->get_score());
                fsm_.game_screen.set_hi_score(pgame_->get_hi_score());
                fsm_.game_screen.create_next_input(pgame_->get_input_tiles());
                fsm_.game_screen.insert_next_input(pgame_->get_input_layout());
                fsm_.game_screen.create_next_input(pgame_->get_next_input_tiles());
                fsm_.game_screen.set_board_tiles(pgame_->get_board_tiles());
                mark_tiles_for_nullification();
                fsm_.game_screen.set_game_over_screen_visible(pgame_->is_game_over());
                fsm_.game_screen.set_visible(true);
            }
            else
            {
                pgame_ = std::make_unique<libgame::game>();

                fsm_.game_screen.set_visible(true);
                modify_game(&libgame::game::start);
            }
        }

        void handle_view_clear_request()
        {
            modify_game(&libgame::game::start);
        }

        void handle_view_move_request(const libview::data_types::move m)
        {
            using move = libview::data_types::move;

            switch(m)
            {
                case move::left_shift:
                    modify_game(&libgame::game::shift_input_left);
                    break;
                case move::right_shift:
                    modify_game(&libgame::game::shift_input_right);
                    break;
                case move::clockwise_rotation:
                    modify_game(&libgame::game::rotate_input);
                    break;
                case move::drop:
                    modify_game(&libgame::game::drop_input_tiles);
                    break;
            }
        }

    private:
        void handle_game_event(const libgame::events::start&)
        {
            fsm_.game_screen.clear();
        }

        void handle_game_event(const libgame::events::score_change& event)
        {
            fsm_.game_screen.set_score(event.score);
        }

        void handle_game_event(const libgame::events::hi_score_change& event)
        {
            fsm_.game_screen.set_hi_score(event.score);
        }

        void handle_game_event(const libgame::events::next_input_creation& event)
        {
            fsm_.game_screen.create_next_input(event.tiles);
        }

        void handle_game_event(const libgame::events::next_input_insertion& event)
        {
            fsm_.game_screen.insert_next_input(event.layout);
            mark_tiles_for_nullification();
            fsm_.database.set_game_state(pgame_->get_state());
        }

        void handle_game_event(const libgame::events::input_layout_change& event)
        {
            fsm_.game_screen.set_input_layout(event.layout);
            mark_tiles_for_nullification();
        }

        void handle_game_event(const libgame::events::input_tile_drop& event)
        {
            fsm_.game_screen.drop_input_tiles(event.drops);
        }

        void handle_game_event(const libgame::events::board_tile_drop& event)
        {
            fsm_.game_screen.drop_board_tiles(event.drops);
        }

        void handle_game_event(const libgame::events::tile_nullification& event)
        {
            fsm_.game_screen.nullify_tiles(event.nullified_tile_coordinates);
        }

        void handle_game_event(const libgame::events::tile_merge& event)
        {
            fsm_.game_screen.merge_tiles(event.merges);
        }

        void handle_game_event(const libgame::events::end_of_game&)
        {
            fsm_.game_screen.set_game_over_screen_visible(true);
            fsm_.database.set_game_state(pgame_->get_state());
        }

        void handle_game_events(const libgame::event_list& events)
        {
            for(const auto& event: events)
            {
                std::visit
                (
                    [this](const auto& event)
                    {
#ifndef NDEBUG
                        std::cout << event << '\n';
#endif
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
        template<class Fn>
        void modify_game(Fn&& fn)
        {
            if(!pgame_)
            {
                return;
            }

            game_events_.clear();
            std::invoke(fn, *pgame_, game_events_);
            handle_game_events(game_events_);
        }

        void mark_tiles_for_nullification()
        {
            targeted_tiles_.clear();
            pgame_->get_targeted_tiles(targeted_tiles_);
            fsm_.game_screen.mark_tiles_for_nullification(targeted_tiles_);
        }

    private:
        fsm& fsm_;
        std::unique_ptr<libgame::game> pgame_;

        //used by modify_game()
        libgame::event_list game_events_;

        //used by mark_tiles_for_nullification()
        libcommon::data_types::tile_coordinate_list targeted_tiles_;
};

} //namespace

#endif
