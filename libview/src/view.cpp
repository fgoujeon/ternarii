#include <libview/view.hpp>
#include "grid.hpp"
#include "fixed_ratio_box.hpp"
#include "libsdl.hpp"
#include <utility>

namespace libview
{

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
        pgrid(std::make_shared<grid>()),
        child(6.0 / 14)
    {
        child.add(pgrid);
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
    fixed_ratio_box child;
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
    pimpl_->process_events();

    int window_width_px;
    int window_height_px;
    SDL_GetWindowSize(pimpl_->pwindow.get(), &window_width_px, &window_height_px);

    const auto margin_pc = 3;
    const auto margin_px = window_height_px * (margin_pc / 100.0);

    //grey background
    SDL_SetRenderDrawColor(pimpl_->prenderer.get(), 0x44, 0x44, 0x44, 0xff);
    SDL_RenderClear(pimpl_->prenderer.get());

    rectangle area;
    area.pos_x_px = margin_px;
    area.pos_y_px = margin_px;
    area.width_px = window_width_px - margin_px * 2;
    area.height_px = window_height_px - margin_px * 2;
    pimpl_->child.draw(*pimpl_->prenderer, area);
    SDL_RenderPresent(pimpl_->prenderer.get());
}

bool view::must_quit() const
{
    return pimpl_->quit;
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

} //namespace view
