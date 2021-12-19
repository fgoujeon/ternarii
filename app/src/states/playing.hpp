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

#ifndef STATES_PLAYING_HPP
#define STATES_PLAYING_HPP

#include "../events.hpp"
#include "../context.hpp"
#include <libgame/game.hpp>
#include <libutil/log.hpp>
#include <optional>

namespace states
{

class playing_impl
{
    public:
        using screen = libview::screens::game;
        using screen_transition = libview::view::screen_transition;

    public:
        playing_impl
        (
            context& ctx,
            screen_transition trans,
            libgame::data_types::stage stage
        );

        ~playing_impl()
        {
            save_game();
        }

        void on_event(const fgfsm::event_ref& event)
        {
            visit
            (
                event,

                [this](const events::iteration& event)
                {
                    pgame_->advance(event.elapsed_s);
                    pscreen_->set_time_s(pgame_->get_state().time_s);
                }
            );
        }

    //Game event handlers
    private:
        void handle_game_event(const libgame::events::start&)
        {
            pscreen_->clear();
        }

        void handle_game_event(const libgame::events::move_count_change& event)
        {
            pscreen_->set_move_count(event.value);
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
            save_game();
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
            pscreen_->merge_tiles
            (
                event.merges,
                event.granite_erosions
            );
        }

        void handle_game_event(const libgame::events::tile_value_change& event)
        {
            pscreen_->change_tiles_value
            (
                event.nullified_tile_coordinate,
                event.changes
            );
        }

        void handle_game_event(const libgame::events::end_of_game&)
        {
            pscreen_->set_game_over_overlay_visible(true);
            save_game();
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
            auto targeted_tiles = pgame_->get_targeted_tiles(pscreen_->get_input_layout());
            pscreen_->mark_tiles_for_nullification(std::move(targeted_tiles));
        }

        void save_game()
        {
            ctx_.database.set_stage_state(stage_, pgame_->get_state());
        }

    private:
        context& ctx_;
        const libgame::data_types::stage stage_;
        std::shared_ptr<libview::screens::game> pscreen_;
        std::unique_ptr<libgame::game> pgame_;

        //used by modify_game()
        libgame::event_list game_events_;
};

class playing
{
    public:
        playing(context& ctx):
            ctx_(ctx)
        {
        }

        void on_entry(const fgfsm::event_ref& event)
        {
            visit
            (
                event,

                [this](const events::play_screen_show_request& event)
                {
                    pimpl_ = std::make_unique<playing_impl>(ctx_, event.transition, event.stage);
                }
            );
        }

        void on_event(const fgfsm::event_ref& event)
        {
            pimpl_->on_event(event);
        }

        void on_exit()
        {
            pimpl_ = nullptr;
        }

    private:
        context& ctx_;
        std::unique_ptr<playing_impl> pimpl_;
};

} //namespace

#endif
