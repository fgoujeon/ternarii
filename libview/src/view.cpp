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
        window
        (
            "Test",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640,
            480,
            SDL_WINDOW_RESIZABLE
        ),
        renderer
        (
            window.ptr,
            -1,
            0
        ),
        child(6.0 / 14)
    {
        child.add(std::make_shared<grid>());
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
    libsdl::window window;
    libsdl::renderer renderer;
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
    SDL_SetWindowSize(pimpl_->window.ptr, width, height);
}

void view::iterate()
{
    pimpl_->process_events();

    int window_width_px;
    int window_height_px;
    SDL_GetWindowSize(pimpl_->window.ptr, &window_width_px, &window_height_px);

    const auto margin_pc = 3;
    const auto margin_px = window_height_px * (margin_pc / 100.0);

    //grey background
    SDL_SetRenderDrawColor(pimpl_->renderer.ptr, 0x44, 0x44, 0x44, 0xff);
    SDL_RenderClear(pimpl_->renderer.ptr);

    rectangle area;
    area.pos_x_px = margin_px;
    area.pos_y_px = margin_px;
    area.width_px = window_width_px - margin_px * 2;
    area.height_px = window_height_px - margin_px * 2;
    pimpl_->child.draw(pimpl_->renderer, area);
    SDL_RenderPresent(pimpl_->renderer.ptr);
}

bool view::must_quit()
{
    return pimpl_->quit;
}

} //namespace view
