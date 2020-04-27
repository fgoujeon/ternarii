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

#include <libdb/database.hpp>
#include <libgame/game.hpp>
#include <libview/view.hpp>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

#ifndef NDEBUG
#include <iostream>
#endif

class app: public Magnum::Platform::Sdl2Application
{
    public:
        explicit app(const Arguments& arguments):
            Magnum::Platform::Sdl2Application
            {
                arguments,
                Configuration{}.setWindowFlags(Configuration::WindowFlag::Resizable)
            },
            database_([this](const libdb::event& event){handle_database_event(event);}),
            view_(make_view_callbacks())
        {
        }

    //Sdl2Application virtual functions
    private:
        void drawEvent() override
        {
            Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);
            view_.draw();
            database_.iterate();
            swapBuffers();
            redraw();
        }

        void viewportEvent(ViewportEvent& event) override
        {
            Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
            view_.set_viewport(event.windowSize());
        }

        void keyPressEvent(KeyEvent& event) override
        {
            view_.handle_key_press(event);
        }

        void mousePressEvent(MouseEvent& event) override
        {
            view_.handle_mouse_press(event);
        }

    private:
        void handle_game_event(const libgame::events::start&)
        {
            view_.clear();
        }

        void handle_game_event(const libgame::events::score_change& event)
        {
            view_.set_score(event.score);
        }

        void handle_game_event(const libgame::events::hi_score_change& event)
        {
            view_.set_hi_score(event.score);
        }

        void handle_game_event(const libgame::events::next_input_creation& event)
        {
            view_.create_next_input(event.tiles[0].value, event.tiles[1].value);
        }

        void handle_game_event(const libgame::events::next_input_insertion& event)
        {
            view_.insert_next_input(event.x_offset, event.rotation);
            database_.set_game_state(pgame_->get_state());
        }

        void handle_game_event(const libgame::events::input_layout_change& event)
        {
            view_.set_input_layout(event.x_offset, event.rotation);
        }

        void handle_game_event(const libgame::events::input_insertion& event)
        {
            view_.insert_input
            (
                event.tile0_dst_column_index,
                event.tile0_dst_row_index,
                event.tile1_dst_column_index,
                event.tile1_dst_row_index
            );
        }

        void handle_game_event(const libgame::events::tile_drop& event)
        {
            view_.drop_tiles(event.drops);
        }

        void handle_game_event(const libgame::events::tile_merge& event)
        {
            view_.merge_tiles(event.merges);
        }

        void handle_game_event(const libgame::events::end_of_game&)
        {
            view_.set_game_over_screen_visible(true);
            database_.set_game_state(pgame_->get_state());
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

    private:
        libview::callback_set make_view_callbacks()
        {
            auto callbacks = libview::callback_set{};
            callbacks.handle_clear_request. assign<&app::handle_view_clear_request> (*this);
            callbacks.handle_move_request.  assign<&app::handle_view_move_request>  (*this);
            return callbacks;
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
                    modify_game(&libgame::game::drop_input);
                    break;
            }
        }

    private:
        void handle_database_event2(const libdb::events::end_of_loading&)
        {
            if(pgame_) return;

            //load game state from database
            const auto& game_state = database_.get_game_state();

            //create game
            pgame_ = std::make_unique<libgame::game>(game_state);

            //initialize view
            view_.set_score(pgame_->get_score());
            view_.set_hi_score(game_state.hi_score);
            view_.create_next_input(game_state.input.tiles[0].value, game_state.input.tiles[1].value);
            view_.insert_next_input(game_state.input.x_offset, game_state.input.rotation);
            view_.create_next_input(game_state.next_input_tiles[0].value, game_state.next_input_tiles[1].value);
            view_.set_board_tiles(game_state.board_tiles);
            view_.set_game_over_screen_visible(pgame_->is_game_over());
            view_.set_visible(true);
        }

        void handle_database_event(const libdb::event& event)
        {
            std::visit
            (
                [this](const auto& event)
                {
                    handle_database_event2(event);
                },
                event
            );
        }

    private:
        libdb::database database_;
        libview::view view_;
        std::unique_ptr<libgame::game> pgame_;

        //used by modify_game()
        libgame::event_list game_events_;
};

MAGNUM_APPLICATION_MAIN(app)
