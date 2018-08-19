#include "view.hpp"

view::view():
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
    )
{
}

void view::set_window_size(const unsigned int width, const unsigned int height)
{
    SDL_SetWindowSize(window.ptr, width, height);
}

void view::iterate()
{
    process_events();

    int window_width;
    int window_height;
    SDL_GetWindowSize(window.ptr, &window_width, &window_height);

    //grey background
    SDL_SetRenderDrawColor(renderer.ptr, 0x44, 0x44, 0x44, 0xff);
    SDL_RenderClear(renderer.ptr);

    //centered box
    {
        const auto margin_px = 40;

        const auto container_width_px = window_width - margin_px * 2;
        const auto container_height_px = window_height - margin_px * 2;
        const auto container_ratio = static_cast<double>(container_width_px) / container_height_px;

        const auto box_ratio = 0.5;
        auto box_width_px = 0;
        auto box_height_px = 0;
        if(container_ratio > box_ratio)
        {
            box_width_px = container_height_px * box_ratio;
            box_height_px = container_height_px;
        }
        else
        {
            box_width_px = container_width_px;
            box_height_px = container_width_px / box_ratio;
        }

        SDL_Rect r;
        r.x = window_width / 2 - box_width_px / 2;
        r.y = window_height / 2 - box_height_px / 2;
        r.w = box_width_px;
        r.h = box_height_px;

        SDL_SetRenderDrawColor(renderer.ptr, 0x69, 0, 0xC2, 255);
        SDL_RenderFillRect(renderer.ptr, &r);
    }

    SDL_RenderPresent(renderer.ptr);
}

bool view::must_quit()
{
    return quit;
}

void view::process_events()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
        }
    }
}
