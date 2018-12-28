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

#include <libview_2p/view.hpp>
#include "grid.hpp"
#include "score_display.hpp"
#include "fps_display.hpp"
#include "game_over_screen.hpp"
#include "label_button.hpp"
#include "utility.hpp"
#include "geometry.hpp"
#include <libsdl.hpp>
#include <chrono>
#include <string>
#include <utility>
#include <iostream>

namespace libview_2p
{

namespace
{
    constexpr auto logical_width = 1600;
    constexpr auto logical_height = 900;
    constexpr auto logical_ratio = static_cast<double>(logical_width) / logical_height;
    constexpr auto player_count = 2;
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
                SDL_RENDERER_PRESENTVSYNC
            )
        ),
        grids_{grid{*prenderer_}, grid{*prenderer_}},
        score_displays_
        {
            score_display
            {
                *prenderer_,
                SDL_Rect{50, 50, 225, 50}
            },
            score_display
            {
                *prenderer_,
                SDL_Rect{1325, 50, 225, 50}
            },
        },
        game_over_screen_
        (
            event_handler_,
            *prenderer_,
            SDL_Rect{325, 110, 951, 200}
        ),
        left_shift_button_
        (
            *prenderer_,
            SDL_Rect{50, 350, 225, 225},
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
            SDL_Rect{1325, 350, 225, 225},
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
            SDL_Rect{1325, 625, 225, 225},
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
            SDL_Rect{50, 625, 225, 225},
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

        auto is_animating = false;
        for(auto& grid: grids_)
        {
            if(grid.is_animating())
            {
                is_animating = true;
            }
        }
        if(!is_animating)
            event_handler_(std::forward<Event>(event));
    }

    void draw(const double ellapsed_time)
    {
        //draw background
        SDL_SetRenderDrawColor(prenderer_.get(), 0x44, 0x44, 0x44, 0xff);
        SDL_RenderClear(prenderer_.get());

        //compute system for letterboxing
        geometry::system sys0;
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

        //draw player 1 tile grid
        {
            geometry::system sys;
            sys.origin.x = sys0.origin.x + (325 * sys0.unit);
            sys.origin.y = sys0.origin.y + (25 * sys0.unit);
            sys.unit = 450.0 / grids_[0].get_logical_width() * sys0.unit;

            grids_[0].draw(*prenderer_, sys, ellapsed_time);
        }

        //draw player 2 tile grid
        {
            geometry::system sys;
            sys.origin.x = sys0.origin.x + (825 * sys0.unit);
            sys.origin.y = sys0.origin.y + (25 * sys0.unit);
            sys.unit = 450.0 / grids_[1].get_logical_width() * sys0.unit;

            grids_[1].draw(*prenderer_, sys, ellapsed_time);
        }

        //draw other children
        {
            //fps_display_.draw(sys0, ellapsed_time);

            for(auto& display: score_displays_)
                display.draw(sys0);

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
    std::array<grid, player_count> grids_;
    std::array<score_display, player_count> score_displays_;
    //fps_display fps_display_;
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
    for(auto& grid: pimpl_->grids_)
        grid.clear();
    pimpl_->game_over_screen_.set_visible(false);
}

void view::set_score(const int player_index, const unsigned int value)
{
    pimpl_->score_displays_[player_index].set_score(value);
}

void view::create_next_input(const int player_index, const unsigned int value0, const unsigned int value1)
{
    pimpl_->grids_[player_index].create_next_input(value0, value1);
}

void view::insert_next_input(const int player_index, const unsigned int x_offset, const unsigned int rotation)
{
    pimpl_->grids_[player_index].insert_next_input(x_offset, rotation);
}

void view::set_input_x_offset(const int player_index, const unsigned int value)
{
    pimpl_->grids_[player_index].set_input_x_offset(value);
}

void view::set_input_rotation(const int player_index, const unsigned int value)
{
    pimpl_->grids_[player_index].set_input_rotation(value);
}

void view::insert_input
(
    const int player_index,
    const unsigned int tile0_dst_column_index,
    const unsigned int tile0_dst_row_index,
    const unsigned int tile1_dst_column_index,
    const unsigned int tile1_dst_row_index
)
{
    pimpl_->grids_[player_index].insert_input
    (
        tile0_dst_column_index,
        tile0_dst_row_index,
        tile1_dst_column_index,
        tile1_dst_row_index
    );
}

void view::drop_tiles(const int player_index, const data_types::tile_drop_list& drops)
{
    pimpl_->grids_[player_index].drop_tiles(drops);
}

void view::merge_tiles(const int player_index, const data_types::tile_merge_list& merges)
{
    pimpl_->grids_[player_index].merge_tiles(merges);
}

void view::set_game_over_screen_visible(const bool visible)
{
    pimpl_->game_over_screen_.set_visible(visible);
}

} //namespace libview_2p
