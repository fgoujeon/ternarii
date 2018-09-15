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

#include <libview/view.hpp>
#include "grid.hpp"
#include "score_display.hpp"
#include "fps_display.hpp"
#include "game_over_screen.hpp"
#include "label_button.hpp"
#include "utility.hpp"
#include "system.hpp"
#include <libsdl.hpp>
#include <chrono>
#include <string>
#include <utility>
#include <iostream>

namespace libview
{

namespace
{
    constexpr auto logical_width = 900;
    constexpr auto logical_height = 1600;
    constexpr auto logical_ratio = static_cast<double>(logical_width) / logical_height;
}

struct view::impl
{
    impl(const event_handler& evt_handler):
        event_handler_(evt_handler),
        pwindow_
        (
            SDL_CreateWindow
            (
                "Ternarii",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                800,
                600,
                SDL_WINDOW_RESIZABLE
            )
        ),
        prenderer_
        (
            SDL_CreateRenderer
            (
                pwindow_.get(),
                -1,
                0
            )
        ),
        grid_(*prenderer_),
        score_display_
        (
            *prenderer_,
            SDL_Rect{150, 50, 600, 100}
        ),
        fps_display_
        (
            *prenderer_,
            SDL_Rect{150, 50, 600, 100}
        ),
        game_over_screen_
        (
            event_handler_,
            *prenderer_,
            SDL_Rect{150, 250, 601, 200}
        ),
        left_shift_button_
        (
            *prenderer_,
            SDL_Rect{50, 1300, 150, 150},
            50,
            "left",
            [this]
            {
                send_move_request(events::left_shift_request{});
            }
        ),
        right_shift_button_
        (
            *prenderer_,
            SDL_Rect{210, 1400, 150, 150},
            50,
            "right",
            [this]
            {
                send_move_request(events::right_shift_request{});
            }
        ),
        drop_button_
        (
            *prenderer_,
            SDL_Rect{540, 1400, 150, 150},
            50,
            "drop",
            [this]
            {
                send_move_request(events::drop_request{});
            }
        ),
        rotation_button_
        (
            *prenderer_,
            SDL_Rect{700, 1300, 150, 150},
            50,
            "rotate",
            [this]
            {
                send_move_request(events::clockwise_rotation_request{});
            }
        )
    {
        SDL_SetRenderDrawBlendMode(prenderer_.get(), SDL_BLENDMODE_BLEND);
    }

    void iterate()
    {
        const auto current_time = std::chrono::steady_clock::now();
        const auto ellapsed_time = std::chrono::duration<double>{current_time - previous_frame_time_};

        process_events();
        draw(ellapsed_time.count());

        previous_frame_time_ = current_time;
    }

    void process_events()
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_LEFT:
                            send_move_request(events::left_shift_request{});
                            break;
                        case SDLK_RIGHT:
                            send_move_request(events::right_shift_request{});
                            break;
                        case SDLK_UP:
                        case SDLK_SPACE:
                            send_move_request(events::clockwise_rotation_request{});
                            break;
                        case SDLK_DOWN:
                            send_move_request(events::drop_request{});
                            break;
                    }
                    break;
                case SDL_QUIT:
                    quit_ = true;
                    break;
            }
        }
    }

    template<class Event>
    void send_move_request(Event&& event)
    {
        /*
        Note: We want to ignore user inputs when we're animating, so that:
        - we don't queue too many animations;
        - user moves only when they knows what they're moving.
        */

        if(!grid_.is_animating())
            event_handler_(std::forward<Event>(event));
    }

    void draw(const double ellapsed_time)
    {
        //draw background
        SDL_SetRenderDrawColor(prenderer_.get(), 0x44, 0x44, 0x44, 0xff);
        SDL_RenderClear(prenderer_.get());

        //compute system for letterboxing
        system sys0;
        {
            auto window_width = 0;
            auto window_height = 0;
            SDL_GetWindowSize(pwindow_.get(), &window_width, &window_height);
            const auto window_ratio = static_cast<double>(window_width) / window_height;

            if(window_ratio > logical_ratio)
            {
                sys0.unit = static_cast<double>(window_height) / logical_height;
                sys0.unit = sys0.unit;
                sys0.origin.x = window_width / 2.0 - (logical_width * sys0.unit) / 2.0;
            }
            else
            {
                sys0.unit = static_cast<double>(window_width) / logical_width;
                sys0.unit = sys0.unit;
                sys0.origin.y = window_height / 2.0 - (logical_height * sys0.unit) / 2.0;
            }
        }

        //draw tile grid
        {
            system sys;
            sys.origin.x = sys0.origin.x + (150 * sys0.unit);
            sys.origin.y = sys0.origin.y + (150 * sys0.unit);
            sys.unit = sys0.unit;
            sys.unit = sys0.unit;

            grid_.draw(*prenderer_, sys, ellapsed_time);
        }

        //draw other children
        {
            fps_display_.draw(sys0, ellapsed_time);
            score_display_.draw(sys0);
            left_shift_button_.draw(sys0);
            right_shift_button_.draw(sys0);
            drop_button_.draw(sys0);
            rotation_button_.draw(sys0);
            game_over_screen_.draw(*prenderer_, sys0);
        }

        SDL_RenderPresent(prenderer_.get());
    }

    event_handler event_handler_;
    libsdl::session session_;
    libsdl::unique_ptr<SDL_Window> pwindow_;
    libsdl::unique_ptr<SDL_Renderer> prenderer_;
    grid grid_;
    score_display score_display_;
    fps_display fps_display_;
    game_over_screen game_over_screen_;

    label_button left_shift_button_;
    label_button right_shift_button_;
    label_button drop_button_;
    label_button rotation_button_;

    std::chrono::time_point<std::chrono::steady_clock> previous_frame_time_;

    bool quit_ = false;
};

view::view(const event_handler& evt_handler):
    pimpl_(std::make_unique<impl>(evt_handler))
{
}

view::~view() = default;

void view::set_window_size(const unsigned int width, const unsigned int height)
{
    SDL_SetWindowSize(pimpl_->pwindow_.get(), width, height);
}

void view::iterate()
{
    pimpl_->iterate();
}

bool view::must_quit() const
{
    return pimpl_->quit_;
}

void view::clear()
{
    pimpl_->grid_.clear();
    pimpl_->game_over_screen_.set_visible(false);
}

void view::set_score(const unsigned int value)
{
    pimpl_->score_display_.set_score(value);
}

void view::create_next_input(const unsigned int value0, const unsigned int value1)
{
    pimpl_->grid_.create_next_input(value0, value1);
}

void view::insert_next_input(const unsigned int x_offset, const unsigned int rotation)
{
    pimpl_->grid_.insert_next_input(x_offset, rotation);
}

void view::set_input_x_offset(const unsigned int value)
{
    pimpl_->grid_.set_input_x_offset(value);
}

void view::set_input_rotation(const unsigned int value)
{
    pimpl_->grid_.set_input_rotation(value);
}

void view::insert_input
(
    const unsigned int tile0_dst_column_index,
    const unsigned int tile0_dst_row_index,
    const unsigned int tile1_dst_column_index,
    const unsigned int tile1_dst_row_index
)
{
    pimpl_->grid_.insert_input
    (
        tile0_dst_column_index,
        tile0_dst_row_index,
        tile1_dst_column_index,
        tile1_dst_row_index
    );
}

void view::drop_tiles(const data_types::tile_drop_list& drops)
{
    pimpl_->grid_.drop_tiles(drops);
}

void view::merge_tiles(const data_types::tile_merge_list& merges)
{
    pimpl_->grid_.merge_tiles(merges);
}

void view::set_game_over_screen_visible(const bool visible)
{
    pimpl_->game_over_screen_.set_visible(visible);
}

} //namespace libview
