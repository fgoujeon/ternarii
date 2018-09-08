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
#include "game_over_screen.hpp"
#include "utility.hpp"
#include <libsdl.hpp>
#include <chrono>
#include <string>
#include <utility>
#include <iostream>

namespace libview
{

namespace
{
    const auto logical_width = 700;
    const auto logical_height = 1300;
}

struct view::impl
{
    impl(const event_handler& evt_handler):
        evt_handler(evt_handler),
        pwindow
        (
            SDL_CreateWindow
            (
                "Ternarii",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                640,
                480,
                SDL_WINDOW_RESIZABLE
            )
        ),
        prenderer
        (
            SDL_CreateRenderer
            (
                pwindow.get(),
                -1,
                0
            )
        ),
        pgrid(std::make_shared<grid>(*prenderer)),
        pscore_display
        (
            std::make_shared<score_display>
            (
                *prenderer,
                SDL_Rect
                {
                    50,
                    50,
                    logical_width - 100,
                    100
                }
            )
        ),
        pgame_over_screen
        (
            std::make_shared<game_over_screen>
            (
                evt_handler,
                *prenderer,
                SDL_Rect
                {
                    50,
                    250,
                    logical_width - 100,
                    200
                }
            )
        )
    {
        SDL_SetRenderDrawBlendMode(prenderer.get(), SDL_BLENDMODE_BLEND);
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
        /*
        Note: We want to ignore user inputs when we're animating, so that:
        - we don't queue to many animations;
        - user moves only when they knows what they're moving.
        */

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                    if(!pgrid->is_animating())
                    {
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_LEFT:
                                evt_handler(events::left_shift_request{});
                                break;
                            case SDLK_RIGHT:
                                evt_handler(events::right_shift_request{});
                                break;
                            case SDLK_UP:
                                evt_handler(events::clockwise_rotation_request{});
                                break;
                            case SDLK_DOWN:
                                evt_handler(events::drop_request{});
                                break;
                        }
                    }
                    break;
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }
    }

    void draw(const double ellapsed_time)
    {
        //draw background
        SDL_SetRenderDrawColor(prenderer.get(), 0x44, 0x44, 0x44, 0xff);
        SDL_RenderClear(prenderer.get());

        SDL_RenderSetLogicalSize
        (
            prenderer.get(),
            logical_width,
            logical_height
        );

        //draw children
        {
            SDL_Rect current_viewport;
            SDL_RenderGetViewport(prenderer.get(), &current_viewport);

            float current_x_scale, current_y_scale;
            SDL_RenderGetScale(prenderer.get(), &current_x_scale, &current_y_scale);

            //tile grid
            {
                const auto grid_logical_width = pgrid->get_logical_width();
                const auto grid_logical_height = pgrid->get_logical_height();

                SDL_Rect viewport;
                viewport.x = current_viewport.x + 50;
                viewport.y = current_viewport.y + 150;
                viewport.w = grid_logical_width;
                viewport.h = grid_logical_height;
                SDL_RenderSetViewport(prenderer.get(), &viewport);

                pgrid->draw(*prenderer, ellapsed_time);

                SDL_RenderSetViewport(prenderer.get(), &current_viewport);
            }

            //score
            pscore_display->draw();

            //game over screen
            {
                pgame_over_screen->draw(*prenderer);
            }
        }

        SDL_RenderPresent(prenderer.get());
    }

    event_handler evt_handler;
    libsdl::session session;
    libsdl::unique_ptr<SDL_Window> pwindow;
    libsdl::unique_ptr<SDL_Renderer> prenderer;
    std::shared_ptr<grid> pgrid;
    std::shared_ptr<score_display> pscore_display;
    std::shared_ptr<game_over_screen> pgame_over_screen;

    std::chrono::time_point<std::chrono::steady_clock> previous_frame_time_;

    bool quit = false;
};

view::view(const event_handler& evt_handler):
    pimpl_(std::make_unique<impl>(evt_handler))
{
}

view::~view() = default;

void view::set_window_size(const unsigned int width, const unsigned int height)
{
    SDL_SetWindowSize(pimpl_->pwindow.get(), width, height);
}

void view::iterate()
{
    pimpl_->iterate();
}

bool view::must_quit() const
{
    return pimpl_->quit;
}

void view::clear()
{
    pimpl_->pgrid->clear();
    pimpl_->pgame_over_screen->set_visible(false);
}

void view::set_score(const unsigned int value)
{
    pimpl_->pscore_display->set_score(value);
}

void view::create_next_input(const unsigned int value0, const unsigned int value1)
{
    pimpl_->pgrid->create_next_input(value0, value1);
}

void view::insert_next_input(const unsigned int x_offset, const unsigned int rotation)
{
    pimpl_->pgrid->insert_next_input(x_offset, rotation);
}

void view::set_input_x_offset(const unsigned int value)
{
    pimpl_->pgrid->set_input_x_offset(value);
}

void view::set_input_rotation(const unsigned int value)
{
    pimpl_->pgrid->set_input_rotation(value);
}

void view::insert_input
(
    const unsigned int tile0_dst_column_index,
    const unsigned int tile0_dst_row_index,
    const unsigned int tile1_dst_column_index,
    const unsigned int tile1_dst_row_index
)
{
    pimpl_->pgrid->insert_input
    (
        tile0_dst_column_index,
        tile0_dst_row_index,
        tile1_dst_column_index,
        tile1_dst_row_index
    );
}

void view::drop_tiles(const data_types::tile_drop_list& drops)
{
    pimpl_->pgrid->drop_tiles(drops);
}

void view::merge_tiles(const data_types::tile_merge_list& merges)
{
    pimpl_->pgrid->merge_tiles(merges);
}

void view::set_game_over_screen_visible(const bool visible)
{
    pimpl_->pgame_over_screen->set_visible(visible);
}

} //namespace libview
