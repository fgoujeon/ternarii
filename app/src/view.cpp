#include "view.hpp"
#include "grid.hpp"

view::view():
    window_
    (
        "Test",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_RESIZABLE
    ),
    renderer_
    (
        window_.ptr,
        -1,
        0
    ),
    child_(0.5)
{
    child_.add(std::make_shared<grid>());
}

void view::set_window_size(const unsigned int width, const unsigned int height)
{
    SDL_SetWindowSize(window_.ptr, width, height);
}

void view::iterate()
{
    process_events();

    int window_width_px;
    int window_height_px;
    SDL_GetWindowSize(window_.ptr, &window_width_px, &window_height_px);

    const auto margin_pc = 3;
    const auto margin_px = window_height_px * (margin_pc / 100.0);

    //grey background
    SDL_SetRenderDrawColor(renderer_.ptr, 0x44, 0x44, 0x44, 0xff);
    SDL_RenderClear(renderer_.ptr);

    rectangle area;
    area.pos_x_px = margin_px;
    area.pos_y_px = margin_px;
    area.width_px = window_width_px - margin_px * 2;
    area.height_px = window_height_px - margin_px * 2;
    child_.draw(renderer_, area);
    SDL_RenderPresent(renderer_.ptr);
}

bool view::must_quit()
{
    return quit_;
}

void view::process_events()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                quit_ = true;
                break;
        }
    }
}
