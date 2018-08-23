#include <libview/view.hpp>
#include "grid.hpp"
#include "score_display.hpp"
#include "game_over_screen.hpp"
#include "utility.hpp"
#include "libsdl.hpp"
#include <string>
#include <utility>
#include <iostream>

namespace libview
{

namespace
{
    const auto logical_width = 900;
    const auto logical_height = 1900;
}

struct view::impl
{
    impl(const callback_set& callbacks):
        callbacks(callbacks),
        pwindow
        (
            SDL_CreateWindow
            (
                "Test",
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
        pscore_display(std::make_shared<score_display>(*prenderer)),
        pgame_over_screen(std::make_shared<game_over_screen>(*prenderer))
    {
        pscore_display->set_area
        (
            SDL_Rect
            {
                static_cast<int>(0.025 * logical_width),
                static_cast<int>(0.025 * logical_height),
                static_cast<int>(0.95 * logical_width),
                120
            }
        );

        pgame_over_screen->set_area
        (
            SDL_Rect
            {
                static_cast<int>(0.02 * logical_width),
                330,
                static_cast<int>(0.96 * logical_width),
                400
            }
        );
    }

    void iterate()
    {
        process_events();

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
                const auto grid_ratio = static_cast<double>(grid_logical_width) / grid_logical_height;

                SDL_Rect viewport;
                viewport.x = current_viewport.x + current_viewport.w * 0.025;
                viewport.y = current_viewport.y + current_viewport.h * 0.085;
                viewport.w = current_viewport.w * 0.95;
                viewport.h = viewport.w / grid_ratio;
                SDL_RenderSetViewport(prenderer.get(), &viewport);

                SDL_RenderSetScale
                (
                    prenderer.get(),
                    current_x_scale * viewport.w / grid_logical_width,
                    current_y_scale * viewport.h / grid_logical_height
                );

                pgrid->draw(*prenderer);
            }

            //score
            {
                SDL_RenderSetScale
                (
                    prenderer.get(),
                    current_x_scale,
                    current_y_scale
                );

                SDL_RenderSetViewport(prenderer.get(), &current_viewport);

                pscore_display->draw(*prenderer);
            }

            //game over screen
            {
                pgame_over_screen->draw(*prenderer);
            }
        }

        SDL_RenderPresent(prenderer.get());
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
                            callbacks.left_shift();
                            break;
                        case SDLK_RIGHT:
                            callbacks.right_shift();
                            break;
                        case SDLK_UP:
                            callbacks.clockwise_rotation();
                            break;
                        case SDLK_DOWN:
                            callbacks.down();
                            break;
                    }
                    break;
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }
    }

    callback_set callbacks;
    libsdl::session session;
    libsdl::window_unique_ptr pwindow;
    libsdl::renderer_unique_ptr prenderer;
    std::shared_ptr<grid> pgrid;
    std::shared_ptr<score_display> pscore_display;
    std::shared_ptr<game_over_screen> pgame_over_screen;

    bool quit = false;
};

view::view(const callback_set& callbacks):
    pimpl_(std::make_unique<impl>(callbacks))
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

void view::set_score(const unsigned int value)
{
    pimpl_->pscore_display->set_score(value);
}

void view::set_next_input_items(const next_input_item_array& items)
{
    pimpl_->pgrid->set_next_input_items(items);
}

void view::set_input_items(const input_item_array& items)
{
    pimpl_->pgrid->set_input_items(items);
}

void view::set_board_items(const board_item_array& items)
{
    pimpl_->pgrid->set_board_items(items);
}

void view::set_game_over_screen_visible(const bool visible)
{
    pimpl_->pgame_over_screen->set_visible(visible);
}

} //namespace view
